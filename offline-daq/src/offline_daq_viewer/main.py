from __future__ import annotations

import sys

from PySide6.QtWidgets import QApplication

from offline_daq_viewer.data.paths import resolve_csv_inputs
from offline_daq_viewer.ui.main_window import MainWindow


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("Offline DAQ Viewer")
    window = MainWindow()
    csv_paths = resolve_csv_inputs(sys.argv[1:])
    if csv_paths:
        window.open_csv(csv_paths[0])
    window.resize(1200, 760)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
