"""Hidden-window component tests with synthetic, temporary CSV fixtures."""
import csv
import sys
import tempfile
import time
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'logger'))
try:
    import tkinter as tk
    import airspeed_logger as app
except ImportError:
    tk = app = None


@unittest.skipIf(tk is None, 'Tk is not installed')
class GuiFeatures(unittest.TestCase):
    def setUp(self):
        try:
            self.root = tk.Tk()
        except tk.TclError as exc:
            self.skipTest(str(exc))
        self.root.withdraw()
        self.temp = tempfile.TemporaryDirectory()
        self.directory = Path(self.temp.name)
        self.fixture = self.directory / 'airspeed-synthetic.csv'
        with self.fixture.open('w', newline='') as output:
            writer = csv.DictWriter(output, fieldnames=['received_utc', 'raw_counts',
                'corrected_pressure_pa', 'airspeed_m_s', 'reading_state', 'zero_offset_pa'])
            writer.writeheader()
            for second, speed in enumerate([0, 5, 10]):
                writer.writerow({'received_utc': f'2026-01-01T22:00:0{second}+00:00',
                                 'raw_counts': 8220, 'corrected_pressure_pa': 10,
                                 'airspeed_m_s': speed, 'reading_state': 'valid', 'zero_offset_pa': 30})
        self.mock = patch.object(app, 'SerialWorker')
        self.mock.start()
        self.gui = app.Logger(self.root, directory=self.directory)

    def tearDown(self):
        if hasattr(self, 'root'):
            self.root.destroy()
        if hasattr(self, 'mock'):
            self.mock.stop()
        if hasattr(self, 'temp'):
            self.temp.cleanup()

    def test_named_start_and_stop_leave_gui_open(self):
        self.gui.title_var.set('Fan test')
        self.gui.start_recording()
        self.assertEqual(self.gui.commands.get_nowait(), {'kind': 'start', 'title': 'Fan test'})
        self.gui.handle_event({'kind': 'recording', 'active': True,
                              'path': str(self.directory / 'example.csv'), 'title': 'Fan test'})
        self.gui.stop_recording()
        self.assertEqual(self.gui.commands.get_nowait()['kind'], 'stop')
        self.gui.handle_event({'kind': 'recording', 'active': False, 'rows': 42,
                              'title': 'Fan test', 'path': str(self.directory / 'example.csv')})
        self.assertFalse(self.gui.recording)
        self.assertTrue(self.root.winfo_exists())

    def test_history_graph_and_twelve_hour_table(self):
        label = next(label for label, path in self.gui.run_lookup.items() if path == self.fixture)
        self.gui.run_var.set(label)
        self.gui.select_run()
        deadline = time.monotonic() + 3
        while not self.gui.history_rows and time.monotonic() < deadline:
            self.root.update()
            time.sleep(.01)
        self.assertEqual(len(self.gui.history_rows), 3)
        self.gui.plot.canvas.winfo_width = lambda: 1100
        self.gui.plot.canvas.winfo_height = lambda: 255
        self.gui.render()
        self.assertEqual(len(self.gui.table.get_children()), 3)
        first = self.gui.table.item(self.gui.table.get_children()[0])['values'][0]
        self.assertRegex(first, r' (AM|PM)$')
        c = self.gui.plot.canvas
        self.assertTrue(any(c.type(item) == 'line' and c.itemcget(item, 'fill') == '#087a98'
                            for item in c.find_all()))
        self.gui.back_to_live()
        self.assertIsNone(self.gui.selected_path)

    def test_rate_control_and_history_during_recording(self):
        self.gui.rate_var.set('500')
        self.gui.apply_rate()
        self.assertEqual(self.gui.commands.get_nowait(), {'kind': 'rate', 'rate': 500})
        self.gui.recording = True
        self.gui.selected_path = self.fixture
        self.gui.render()
        self.assertIn('live recording continues', self.gui.view_text.get())


if __name__ == '__main__':
    unittest.main()
