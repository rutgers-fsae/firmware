#!/usr/bin/env python3
"""Selectable sample rates, named CSV runs, run history, and time-series plots."""
import argparse
import fcntl
import json
import queue
import signal
import sys
import threading
import time
from collections import deque
from datetime import datetime
from pathlib import Path
import tkinter as tk
from tkinter import ttk, messagebox

BASE = Path(__file__).resolve().parent
RUNS = BASE / 'runs'
sys.path.insert(0, str(BASE))
from airspeed_core import (calculate, pressure, format_local, list_runs, load_run,
                           row_time, plot_segments, validate_rate, QUANTUM_PA)
from airspeed_serial import SerialWorker

FRIENDLY = {'needs_zero': 'Needs zero', 'within_zero_noise': 'Within zero noise',
            'negative_pressure_check_tubing': 'Negative pressure — check tubing',
            'valid': 'Above noise band', 'read_failed': 'Sensor did not respond',
            'i2c_recovery': 'I2C recovery', 'stale': 'Stale sensor sample'}
PAGE_SIZE = 100


class Plot(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.canvas = tk.Canvas(self, background='#ffffff', highlightthickness=1,
                                highlightbackground='#d3dbe3', height=200)
        self.canvas.pack(fill='both', expand=True)
        self.rows, self.field = [], 'airspeed_m_s'
        self.canvas.bind('<Configure>', lambda event: self.draw())

    def set_data(self, rows, field):
        self.rows, self.field = rows, field
        self.draw()

    def draw(self):
        c = self.canvas
        c.delete('all')
        width, height = c.winfo_width(), c.winfo_height()
        if width < 200 or height < 100:
            return
        left, right, top, bottom = 70, width - 30, 35, height - 55
        is_speed = self.field == 'airspeed_m_s'
        title = 'Airspeed (m/s)' if is_speed else 'Corrected pressure (Pa)'
        c.create_text(14, 12, text=title, anchor='nw', fill='#223244', font=('Helvetica', 12, 'bold'))
        segments = plot_segments(self.rows, self.field, maximum_points=max(500, width * 2))
        points = [point for segment in segments for point in segment]
        if not points:
            c.create_text(width / 2, height / 2, text='No calculated values yet. Select a saved run, or zero the live sensor.',
                          fill='#66778a', width=width - 100, font=('Helvetica', 12))
            return
        xmin, xmax = min(x for x, y in points), max(x for x, y in points)
        xmax = max(xmax, xmin + .001)
        ymin, ymax = min(y for x, y in points), max(y for x, y in points)
        if is_speed:
            ymin, ymax = 0, max(1, ymax * 1.10)
        else:
            pad = max(1, (ymax - ymin) * .1)
            ymin, ymax = ymin - pad, ymax + pad
        def px(x): return left + (x - xmin) / (xmax - xmin) * (right - left)
        def py(y): return bottom - (y - ymin) / (ymax - ymin) * (bottom - top)
        for i in range(5):
            y = ymin + (ymax - ymin) * i / 4
            c.create_line(left, py(y), right, py(y), fill='#e5eaf0')
            c.create_text(left - 10, py(y), text=f'{y:.1f}', anchor='e', fill='#526477', font=('Helvetica', 10))
        ticks = 4 if width < 950 else 5
        for i in range(ticks):
            x = xmin + (xmax - xmin) * i / (ticks - 1)
            text = datetime.fromtimestamp(x).strftime('%I:%M:%S %p').lstrip('0')
            c.create_text(px(x), bottom + 18, text=text, fill='#526477', font=('Helvetica', 10))
        c.create_line(left, top, left, bottom, right, bottom, fill='#aab6c4')
        c.create_text((left + right) / 2, height - 10, text='Local time', fill='#526477', font=('Helvetica', 10))
        for segment in segments:
            coords = [value for x, y in segment for value in (px(x), py(y))]
            if len(segment) >= 2:
                c.create_line(*coords, fill='#087a98', width=2)
            else:
                x, y = coords
                c.create_oval(x - 2, y - 2, x + 2, y + 2, fill='#087a98', outline='')


class Logger:
    def __init__(self, root, calibration_path=None, directory=RUNS, port_override=None):
        self.root, self.directory = root, Path(directory)
        root.title('Arduino Airspeed Logger')
        root.geometry('1180x860')
        root.minsize(1050, 820)
        self.commands, self.events = queue.Queue(), queue.Queue()
        self.batches = queue.Queue(maxsize=8)
        self.live_rows, self.history_rows = deque(maxlen=60000), []
        self.selected_path = self.active_path = None
        self.active_title = ''
        self.recording = self.connected = self.closing = False
        self.firmware = self.cal = None
        self.page = self.loading_id = 0
        self.last_render = 0
        self.dirty = True
        self.run_lookup = {}
        defaults = {'status': 'Connecting to Nano R4…', 'rate_status': 'Measured rate: waiting for readings',
                    'title_var': 'Airspeed test', 'rate_var': '20', 'run_var': 'Live preview',
                    'metric_var': 'Airspeed (m/s)', 'raw_text': '—', 'pa_text': '—', 'speed_text': '—',
                    'cal_text': 'Zero in still air before calculating speed.',
                    'count_text': 'Not recording. Enter a title and press Start recording.',
                    'view_text': 'Live preview · most recent 60 seconds', 'page_text': 'No rows',
                    'file_text': 'CSV timestamps remain UTC. GUI times use AM/PM.'}
        for name, value in defaults.items():
            setattr(self, name, tk.StringVar(value=value))
        self._build()
        calibration = json.loads(Path(calibration_path).read_text()) if calibration_path else None
        self.worker = SerialWorker(self.directory, self.commands, self.events, self.batches,
                                   calibration=calibration, port_override=port_override)
        self.worker.start()
        self.refresh_runs()
        root.protocol('WM_DELETE_WINDOW', self.close)
        root.after(50, self.poll_events)

    def _build(self):
        root = ttk.Frame(self.root, padding=18)
        root.pack(fill='both', expand=True)
        ttk.Label(root, text='Airspeed recorder', font=('Helvetica', 24, 'bold')).pack(anchor='w')
        ttk.Label(root, textvariable=self.status, wraplength=1110).pack(anchor='w', pady=(3, 10))
        recording = ttk.Frame(root)
        recording.pack(fill='x', pady=(0, 8))
        ttk.Label(recording, text='Run title').pack(side='left')
        self.title_entry = ttk.Entry(recording, textvariable=self.title_var, width=35)
        self.title_entry.pack(side='left', padx=(8, 12), fill='x', expand=True)
        self.start_button = ttk.Button(recording, text='Start recording', command=self.start_recording)
        self.start_button.pack(side='left', padx=4)
        self.stop_button = ttk.Button(recording, text='Stop and save', command=self.stop_recording, state='disabled')
        self.stop_button.pack(side='left', padx=4)
        rate = ttk.Frame(root)
        rate.pack(fill='x', pady=(0, 5))
        ttk.Label(rate, text='Reads per second').pack(side='left')
        self.rate_box = ttk.Spinbox(rate, from_=1, to=1000, textvariable=self.rate_var, width=7)
        self.rate_box.pack(side='left', padx=8)
        self.apply_button = ttk.Button(rate, text='Apply rate', command=self.apply_rate)
        self.apply_button.pack(side='left')
        ttk.Label(rate, textvariable=self.rate_status).pack(side='left', padx=14)
        ttk.Label(root, text='Range: 1–1,000/s. Start at 20/s; 500–1,000/s is experimental. Measured fresh rate may be lower.').pack(anchor='w')
        select = ttk.Frame(root)
        select.pack(fill='x', pady=(14, 8))
        ttk.Label(select, text='View run').pack(side='left')
        self.run_box = ttk.Combobox(select, textvariable=self.run_var, state='readonly', width=60)
        self.run_box.pack(side='left', fill='x', expand=True, padx=8)
        self.run_box.bind('<<ComboboxSelected>>', lambda event: self.select_run())
        ttk.Button(select, text='Refresh runs', command=self.refresh_runs).pack(side='left', padx=4)
        ttk.Button(select, text='Back to live', command=self.back_to_live).pack(side='left', padx=4)
        metrics = ttk.Frame(root)
        metrics.pack(fill='x')
        for title, variable in [('Raw count', self.raw_text), ('Corrected pressure', self.pa_text), ('Estimated airspeed', self.speed_text)]:
            group = ttk.Frame(metrics)
            group.pack(side='left', fill='x', expand=True)
            ttk.Label(group, text=title).pack(anchor='w')
            ttk.Label(group, textvariable=variable, font=('Helvetica', 20)).pack(anchor='w', pady=(2, 5))
        zero_frame = ttk.Frame(root)
        zero_frame.pack(fill='x', pady=(2, 8))
        self.zero_button = ttk.Button(zero_frame, text='Zero in still air', command=self.start_zero)
        self.zero_button.pack(side='left')
        ttk.Label(zero_frame, textvariable=self.cal_text, wraplength=880).pack(side='left', padx=12)
        graph_header = ttk.Frame(root)
        graph_header.pack(fill='x', pady=(0, 5))
        ttk.Label(graph_header, textvariable=self.view_text).pack(side='left')
        metric = ttk.Combobox(graph_header, textvariable=self.metric_var,
                             values=['Airspeed (m/s)', 'Corrected pressure (Pa)'], width=24, state='readonly')
        metric.pack(side='right')
        metric.bind('<<ComboboxSelected>>', lambda event: self.mark_dirty())
        self.plot = Plot(root)
        self.plot.pack(fill='both', expand=True)
        table_frame = ttk.Frame(root)
        table_frame.pack(fill='both', expand=True, pady=(10, 4))
        self.table = ttk.Treeview(table_frame, columns=('time', 'raw', 'pa', 'speed', 'state'), show='headings', height=6)
        for key, label, width in [('time', 'Local time (AM/PM)', 170), ('raw', 'Raw count', 90),
                                  ('pa', 'Corrected Pa', 125), ('speed', 'Speed · m/s', 120), ('state', 'Reading state', 360)]:
            self.table.heading(key, text=label)
            self.table.column(key, width=width, anchor='w')
        scrollbar = ttk.Scrollbar(table_frame, orient='vertical', command=self.table.yview)
        self.table.configure(yscrollcommand=scrollbar.set)
        self.table.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        pagination = ttk.Frame(root)
        pagination.pack(fill='x')
        ttk.Button(pagination, text='Newer rows', command=lambda: self.change_page(-1)).pack(side='left')
        ttk.Button(pagination, text='Older rows', command=lambda: self.change_page(1)).pack(side='left', padx=6)
        ttk.Label(pagination, textvariable=self.page_text).pack(side='left', padx=8)
        ttk.Label(root, textvariable=self.count_text).pack(anchor='w', pady=(8, 1))
        ttk.Label(root, textvariable=self.file_text, wraplength=1110).pack(anchor='w')
        ttk.Label(root, text='Density: 1.225 kg/m³. Speed within the zero-noise band is logged as 0 and flagged. Keep Serial Monitor closed.').pack(anchor='w', pady=(5, 0))

    def mark_dirty(self):
        self.dirty = True

    def start_recording(self):
        if not self.recording:
            self.commands.put({'kind': 'start', 'title': self.title_var.get().strip() or 'Untitled run'})
            self.start_button.state(['disabled'])

    def stop_recording(self):
        self.commands.put({'kind': 'stop'})

    def apply_rate(self):
        try:
            rate = validate_rate(self.rate_var.get())
        except ValueError as exc:
            messagebox.showerror('Read rate', str(exc))
            return
        self.commands.put({'kind': 'rate', 'rate': rate})
        self.rate_status.set(f'Requested {rate}/s; waiting for acknowledgement…')

    def start_zero(self):
        if self.selected_path is not None:
            messagebox.showinfo('Live sensor', 'Click Back to live before zeroing the sensor.')
            return
        if messagebox.askokcancel('Zero in still air', 'Put both ports at equal pressure in still air. Do not squeeze the tubing. Collect at least 20 readings over at least 5 seconds?'):
            self.commands.put({'kind': 'zero'})

    def refresh_runs(self):
        current = self.run_var.get()
        self.run_lookup = {'Live preview': None}
        labels = ['Live preview']
        for run in list_runs(self.directory):
            label, counter = run['label'], 2
            while label in self.run_lookup:
                label = run['label'] + f' ({counter})'
                counter += 1
            labels.append(label)
            self.run_lookup[label] = run['path']
        self.run_box['values'] = labels
        if current not in self.run_lookup:
            self.run_var.set('Live preview')

    def select_run(self):
        path = self.run_lookup.get(self.run_var.get())
        if path is None:
            self.back_to_live()
            return
        self.selected_path, self.page, self.history_rows = path, 0, []
        self.view_text.set('Loading saved run…')
        self.loading_id += 1
        request_id = self.loading_id
        self.zero_button.state(['disabled'])
        def load():
            try:
                rows = load_run(path)
                for row in rows:
                    try: row['_plot_time'] = row_time(row)
                    except (KeyError, ValueError): pass
                self.events.put({'kind': 'history', 'request_id': request_id, 'rows': rows, 'path': path})
            except (OSError, ValueError) as exc:
                self.events.put({'kind': 'history_error', 'request_id': request_id, 'message': str(exc)})
        threading.Thread(target=load, daemon=True).start()
        self.dirty = True

    def back_to_live(self):
        self.loading_id += 1
        self.selected_path = None
        self.run_var.set('Live preview')
        self.page = 0
        self.zero_button.state(['!disabled'])
        self.update_cal_label()
        self.dirty = True

    def change_page(self, direction):
        rows = self.history_rows if self.selected_path else self.live_rows
        self.page = max(0, min(max(0, (len(rows) - 1) // PAGE_SIZE), self.page + direction))
        self.dirty = True

    def update_cal_label(self):
        if self.cal:
            band = max(3 * self.cal['noise_std_pa'], 2 * QUANTUM_PA)
            self.cal_text.set(f"Zero: {self.cal['zero_offset_pa']:.2f} Pa · noise band: ±{band:.2f} Pa")
        else:
            self.cal_text.set('Needs zero. Pressure is available; calculated speed stays blank.')

    def handle_event(self, event):
        kind = event['kind']
        if kind == 'notice':
            self.status.set(event['message'])
        elif kind == 'connection':
            self.connected = event['connected']
            if not self.connected:
                self.status.set('Nano disconnected. Saved runs remain available.')
        elif kind == 'firmware':
            self.firmware = event['version']
            self.status.set('Nano connected · variable-rate firmware ready' if self.firmware == 2 else 'Older firmware: readings work at 1/s. Upload the new sketch to enable the rate control.')
        elif kind == 'rate_pending':
            self.rate_status.set(f"Requested {event['rate']}/s; waiting for acknowledgement…")
        elif kind == 'rate_applied':
            self.status.set(f"Arduino accepted {event['rate']} reads per second")
        elif kind == 'recording':
            self.recording = event['active']
            if self.recording:
                self.active_path, self.active_title = Path(event['path']), event['title']
                self.live_rows.clear()
                self.back_to_live()
                self.title_entry.state(['disabled'])
                self.start_button.state(['disabled'])
                self.stop_button.state(['!disabled'])
                self.file_text.set('Saving: ' + self.active_path.name)
            else:
                self.count_text.set(f"Saved {event['rows']:,} rows · {event['title']}")
                self.title_entry.state(['!disabled'])
                self.start_button.state(['!disabled'])
                self.stop_button.state(['disabled'])
            self.refresh_runs()
        elif kind == 'calibration':
            self.cal = event['calibration']
            if self.selected_path is None:
                self.update_cal_label()
                if not self.cal: self.cal_text.set(event['message'])
        elif kind == 'snapshot':
            self.connected, self.firmware = event['connected'], event['version']
            rate = event['rate']
            self.rate_status.set(f"Applied: {rate if rate is not None else '—'}/s · received: {event['attempted_hz']:.1f}/s · fresh: {event['fresh_hz']:.1f}/s · skipped slots: {event.get('skipped', 0)}")
            if event['rows'] is not None:
                self.count_text.set(f"Recording {self.active_title} · {event['rows']:,} rows saved · {event['missing']} serial samples missed")
            if event['zero_count'] is not None and self.selected_path is None:
                self.cal_text.set(f"Zeroing: {event['zero_count']} readings, {event['zero_elapsed']:.1f}s. Keep still.")
            if self.connected and event['stale']:
                self.status.set('USB connected, but no fresh serial samples for 5 seconds.')
                if self.selected_path is None:
                    self.raw_text.set('—'); self.pa_text.set('—'); self.speed_text.set('—')
            elif event.get('last'):
                state = event['last'].get('reading_state', '')
                if state == 'read_failed':
                    self.status.set('Arduino connected; the pressure sensor is not responding at 0x28.')
                elif self.firmware == 2:
                    self.status.set('Nano connected · ' + FRIENDLY.get(state, state.replace('_', ' ')))
        elif kind == 'history' and event['request_id'] == self.loading_id:
            self.history_rows = event['rows']
            self.file_text.set('Viewing: ' + event['path'].name)
            self.dirty = True
        elif kind == 'history_error' and event['request_id'] == self.loading_id:
            messagebox.showerror('Unable to load run', event['message'])
        elif kind in ('error', 'fatal'):
            messagebox.showerror('Airspeed logger', event['message'])
            if not self.recording: self.start_button.state(['!disabled'])
            if kind == 'fatal': self.status.set(event['message'])
        elif kind == 'stopped' and self.closing:
            self.root.destroy()

    def render(self):
        rows = self.history_rows if self.selected_path else list(self.live_rows)
        if self.selected_path:
            self.view_text.set(f'Saved run · {len(rows):,} rows' + (' · live recording continues' if self.recording else ''))
        else:
            self.view_text.set(('Live recording' if self.recording else 'Live preview (not saving)') + ' · graph shows the most recent 60 seconds')
            self.file_text.set(('Saving: ' + self.active_path.name) if self.recording else 'New recordings are saved in the outputs folder. CSV timestamps remain UTC.')
        if rows:
            last = rows[-1]
            raw_value = last.get('raw_counts', '')
            self.raw_text.set(str(raw_value) if raw_value not in ('', None) else '—')
            pa, speed = last.get('corrected_pressure_pa', ''), last.get('airspeed_m_s', '')
            self.pa_text.set(f'{float(pa):.2f} Pa' if pa not in ('', None) else '—')
            self.speed_text.set('Within zero noise' if last.get('reading_state') == 'within_zero_noise' else f'{float(speed):.2f} m/s' if speed not in ('', None) else '—')
            if self.selected_path:
                offset = last.get('zero_offset_pa', '')
                self.cal_text.set(f'Historical zero offset: {float(offset):.2f} Pa' if offset else 'This row has no active zero calibration.')
        else:
            self.raw_text.set('—'); self.pa_text.set('—'); self.speed_text.set('—')
        field = 'airspeed_m_s' if self.metric_var.get() == 'Airspeed (m/s)' else 'corrected_pressure_pa'
        graph_rows = rows
        if not self.selected_path and rows:
            cutoff = max((r.get('_plot_time', 0) for r in rows), default=0) - 60
            graph_rows = [r for r in rows if r.get('_plot_time', 0) >= cutoff]
        self.plot.set_data(graph_rows, field)
        children = self.table.get_children()
        if children: self.table.delete(*children)
        end = max(0, len(rows) - self.page * PAGE_SIZE)
        start = max(0, end - PAGE_SIZE)
        for row in reversed(rows[start:end]):
            def number(key):
                value = row.get(key, '')
                return f'{float(value):.3f}' if value not in ('', None) else '—'
            when = row.get('sample_utc') or row.get('received_utc')
            try: when = format_local(when, milliseconds=True)
            except ValueError: when = '—'
            state = row.get('reading_state', '')
            raw_value = row.get('raw_counts', '')
            self.table.insert('', 'end', values=(when, raw_value if raw_value not in ('', None) else '—', number('corrected_pressure_pa'),
                              number('airspeed_m_s'), FRIENDLY.get(state, state.replace('_', ' '))))
        self.page_text.set(f'Rows {start + 1 if rows else 0:,}–{end:,} of {len(rows):,}, newest first. CSV keeps every sample.')

    def poll_events(self):
        if not self.root.winfo_exists(): return
        while not self.events.empty():
            event = self.events.get_nowait()
            self.handle_event(event)
            if event['kind'] == 'stopped' and self.closing: return
        while not self.batches.empty():
            for row in self.batches.get_nowait():
                try: row['_plot_time'] = row_time(row)
                except (KeyError, ValueError): row['_plot_time'] = time.time()
                self.live_rows.append(row)
            if self.selected_path is None: self.dirty = True
        if self.dirty and time.monotonic() - self.last_render >= .25:
            self.render()
            self.dirty = False
            self.last_render = time.monotonic()
        self.root.after(50, self.poll_events)

    def close(self):
        if self.closing: return
        self.closing = True
        self.status.set('Saving the recording and closing…')
        self.commands.put({'kind': 'shutdown'})
        if not self.worker.is_alive(): self.root.destroy()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--calibration', help='Explicitly reuse a calibration from the same board')
    parser.add_argument('--data-dir', type=Path, default=RUNS)
    parser.add_argument('--port', help='Explicit serial port for development/simulator testing')
    args = parser.parse_args()
    args.data_dir.mkdir(parents=True, exist_ok=True)
    root = tk.Tk()
    instance_lock = (args.data_dir / '.airspeed-logger.lock').open('a')
    try:
        fcntl.flock(instance_lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        root.withdraw()
        messagebox.showinfo('Already running', 'The airspeed logger is already open. Use that window so only one logger owns the USB port.')
        root.destroy()
        raise SystemExit(0)
    app = Logger(root, args.calibration, directory=args.data_dir, port_override=args.port)
    signal.signal(signal.SIGTERM, lambda signum, frame: root.after(0, app.close))
    try:
        root.mainloop()
    except KeyboardInterrupt:
        app.commands.put({'kind': 'shutdown'})
        app.worker.join(timeout=3)
