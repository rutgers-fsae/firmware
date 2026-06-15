from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Callable

from PySide6.QtCore import QObject, QThread, Signal
from PySide6.QtWidgets import (
    QComboBox,
    QFileDialog,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QSpinBox,
    QStatusBar,
    QWidget,
)

from offline_daq_viewer.data.csv_loader import CsvSchema, LoadedColumns, load_columns, read_schema
from offline_daq_viewer.data.paths import default_csv_dir
from offline_daq_viewer.plot.plot_widget import CsvPlotWidget


@dataclass(frozen=True)
class WorkerResult:
    request_id: int
    value: object


class Worker(QObject):
    finished = Signal(object)
    failed = Signal(int, str)

    def __init__(self, request_id: int, task: Callable[[], object]) -> None:
        super().__init__()
        self._request_id = request_id
        self._task = task

    def run(self) -> None:
        try:
            self.finished.emit(WorkerResult(self._request_id, self._task()))
        except Exception as exc:
            self.failed.emit(self._request_id, str(exc))


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("Offline DAQ Viewer")
        self._csv_path: Path | None = None
        self._request_id = 0
        self._threads: list[QThread] = []

        self._file_label = QLabel("No CSV selected")
        self._choose_file = QPushButton("Choose CSV")
        self._x_combo = QComboBox()
        self._y_combo = QComboBox()
        self._plot_button = QPushButton("Plot")
        self._max_points = QSpinBox()
        self._status = QStatusBar()
        self._plot = CsvPlotWidget()

        self._configure_widgets()
        self._wire_events()
        self.setStatusBar(self._status)
        self.setCentralWidget(self._build_layout())
        self._set_ready(False)

    def _configure_widgets(self) -> None:
        self._file_label.setMinimumWidth(320)
        self._x_combo.setMinimumWidth(220)
        self._y_combo.setMinimumWidth(220)
        self._max_points.setRange(1_000, 500_000)
        self._max_points.setSingleStep(5_000)
        self._max_points.setValue(50_000)
        self._max_points.setSuffix(" pts")
        self._status.showMessage("Choose a CSV file to begin.")

    def _wire_events(self) -> None:
        self._choose_file.clicked.connect(self._choose_csv)
        self._plot_button.clicked.connect(self._plot_selected_columns)

    def _build_layout(self) -> QWidget:
        root = QWidget()
        layout = QGridLayout(root)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setHorizontalSpacing(10)
        layout.setVerticalSpacing(10)

        file_row = QHBoxLayout()
        file_row.addWidget(self._choose_file)
        file_row.addWidget(self._file_label, stretch=1)

        layout.addLayout(file_row, 0, 0, 1, 7)
        layout.addWidget(QLabel("X"), 1, 0)
        layout.addWidget(self._x_combo, 1, 1)
        layout.addWidget(QLabel("Y"), 1, 2)
        layout.addWidget(self._y_combo, 1, 3)
        layout.addWidget(QLabel("Max plot points"), 1, 4)
        layout.addWidget(self._max_points, 1, 5)
        layout.addWidget(self._plot_button, 1, 6)
        layout.addWidget(self._plot, 2, 0, 1, 7)
        layout.setRowStretch(2, 1)
        layout.setColumnStretch(3, 1)
        return root

    def _choose_csv(self) -> None:
        file_name, _ = QFileDialog.getOpenFileName(
            self,
            "Choose CSV",
            str(default_csv_dir()),
            "CSV files (*.csv);;All files (*)",
        )
        if not file_name:
            return

        self.open_csv(Path(file_name))

    def open_csv(self, path: str | Path) -> None:
        self._csv_path = Path(path).expanduser().resolve()
        self._file_label.setText(self._csv_path.name)
        self._x_combo.clear()
        self._y_combo.clear()
        self._plot.clear()
        self._set_ready(False)
        self._status.showMessage("Reading CSV headers...")
        csv_path = self._csv_path
        self._run_worker(lambda path=csv_path: read_schema(path), self._schema_loaded, "schema")

    def _schema_loaded(self, schema: CsvSchema) -> None:
        self._x_combo.addItems(schema.columns)
        self._y_combo.addItems(schema.columns)
        if len(schema.columns) > 1:
            self._y_combo.setCurrentIndex(1)
        self._set_ready(len(schema.columns) > 1)
        self._status.showMessage(f"Loaded {len(schema.columns)} columns from {schema.path.name}.")

    def _plot_selected_columns(self) -> None:
        if self._csv_path is None:
            return
        x_column = self._x_combo.currentText()
        y_column = self._y_combo.currentText()
        if not x_column or not y_column:
            return
        if x_column == y_column:
            QMessageBox.warning(self, "Invalid columns", "Choose two different columns.")
            return

        self._set_ready(False)
        self._status.showMessage(f"Loading {x_column} vs {y_column}...")
        csv_path = self._csv_path
        self._run_worker(
            lambda path=csv_path, x=x_column, y=y_column: load_columns(path, x, y),
            self._columns_loaded,
            "columns",
        )

    def _columns_loaded(self, columns: LoadedColumns) -> None:
        original, plotted = self._plot.plot_columns(columns, self._max_points.value())
        self._set_ready(True)
        self._status.showMessage(
            f"Plotted {plotted:,} of {original:,} valid points. "
            f"Dropped {columns.dropped_rows:,} invalid rows from {columns.total_rows:,} selected rows."
        )

    def _run_worker(self, task: Callable[[], object], on_success: Callable[[object], None], label: str) -> None:
        self._request_id += 1
        request_id = self._request_id
        thread = QThread(self)
        worker = Worker(request_id, task)
        worker.moveToThread(thread)

        def finish(result: WorkerResult) -> None:
            if result.request_id == self._request_id:
                on_success(result.value)

        def fail(failed_request_id: int, message: str) -> None:
            if failed_request_id != self._request_id:
                return
            self._set_ready(self._x_combo.count() > 1)
            self._status.showMessage(f"{label.capitalize()} failed.")
            QMessageBox.critical(self, "CSV error", message)

        def cleanup() -> None:
            if thread in self._threads:
                self._threads.remove(thread)

        thread.started.connect(worker.run)
        worker.finished.connect(finish)
        worker.failed.connect(fail)
        worker.finished.connect(thread.quit)
        worker.failed.connect(thread.quit)
        thread.finished.connect(worker.deleteLater)
        thread.finished.connect(cleanup)
        thread.finished.connect(thread.deleteLater)
        self._threads.append(thread)
        thread.start()

    def _set_ready(self, ready: bool) -> None:
        self._x_combo.setEnabled(ready)
        self._y_combo.setEnabled(ready)
        self._plot_button.setEnabled(ready)
        self._max_points.setEnabled(ready)
