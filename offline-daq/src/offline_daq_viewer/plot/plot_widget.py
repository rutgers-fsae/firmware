from __future__ import annotations

import pyqtgraph as pg
from PySide6.QtWidgets import QVBoxLayout, QWidget

from offline_daq_viewer.data.csv_loader import LoadedColumns
from offline_daq_viewer.data.downsample import downsample_min_max


class CsvPlotWidget(QWidget):
    def __init__(self, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self._layout = QVBoxLayout(self)
        self._layout.setContentsMargins(0, 0, 0, 0)
        self._plot: pg.PlotWidget | None = None
        self._build_plot(datetime_axis=False)

    def plot_columns(self, columns: LoadedColumns, max_points: int) -> tuple[int, int]:
        self._build_plot(datetime_axis=columns.x_is_datetime)
        assert self._plot is not None

        sampled = downsample_min_max(columns.x, columns.y, max_points=max_points)
        pen = pg.mkPen(color=(39, 118, 178), width=1.4)
        self._plot.plot(sampled.x, sampled.y, pen=pen)
        self._plot.setLabel("bottom", columns.x_label)
        self._plot.setLabel("left", columns.y_label)
        self._plot.showGrid(x=True, y=True, alpha=0.25)
        self._plot.enableAutoRange()
        return sampled.original_points, sampled.plotted_points

    def clear(self) -> None:
        if self._plot is not None:
            self._plot.clear()

    def _build_plot(self, datetime_axis: bool) -> None:
        if self._plot is not None:
            self._layout.removeWidget(self._plot)
            self._plot.deleteLater()

        axis_items = {"bottom": pg.DateAxisItem()} if datetime_axis else None
        self._plot = pg.PlotWidget(axisItems=axis_items)
        self._plot.setBackground("w")
        self._layout.addWidget(self._plot)

