from __future__ import annotations

from pathlib import Path

import pytest

from offline_daq_viewer.data.csv_loader import CsvLoadError, load_columns, read_schema


def write_csv(path: Path, text: str) -> Path:
    path.write_text(text, encoding="utf-8")
    return path


def test_read_schema_returns_headers(tmp_path: Path) -> None:
    csv_path = write_csv(tmp_path / "sample.csv", "time,rpm,temp\n0,1000,80\n")

    schema = read_schema(csv_path)

    assert schema.columns == ["time", "rpm", "temp"]


def test_load_columns_drops_invalid_rows(tmp_path: Path) -> None:
    csv_path = write_csv(
        tmp_path / "sample.csv",
        "time,rpm\n0,1000\n1,bad\n2,1200\n3,\n",
    )

    loaded = load_columns(csv_path, "time", "rpm")

    assert loaded.total_rows == 4
    assert loaded.valid_rows == 2
    assert loaded.dropped_rows == 2
    assert loaded.x.tolist() == [0.0, 2.0]
    assert loaded.y.tolist() == [1000.0, 1200.0]


def test_load_columns_accepts_datetime_x(tmp_path: Path) -> None:
    csv_path = write_csv(
        tmp_path / "sample.csv",
        "time,rpm\n2026-06-01T12:00:00,1000\n2026-06-01T12:00:01,1100\n",
    )

    loaded = load_columns(csv_path, "time", "rpm")

    assert loaded.x_is_datetime is True
    assert loaded.valid_rows == 2
    assert loaded.x[1] - loaded.x[0] == pytest.approx(1.0)


def test_motec_csv_preamble_uses_channel_header(tmp_path: Path) -> None:
    csv_path = write_csv(
        tmp_path / "motec.csv",
        '"Format","MoTeC CSV File",,,"Workbook",""\n'
        '"Venue","BeaveRun",,,"Worksheet",""\n'
        "\n"
        '"Time","Battery Temp","Motor Speed"\n'
        '"s","C","rpm"\n'
        "\n"
        '"0.000","17.0","1000"\n'
        '"0.040","17.1","1010"\n',
    )

    schema = read_schema(csv_path)
    loaded = load_columns(csv_path, "Time", "Motor Speed")

    assert schema.columns == ["Time", "Battery Temp", "Motor Speed"]
    assert loaded.valid_rows == 2
    assert loaded.x.tolist() == [0.0, 0.04]
    assert loaded.y.tolist() == [1000.0, 1010.0]


def test_load_columns_rejects_same_column(tmp_path: Path) -> None:
    csv_path = write_csv(tmp_path / "sample.csv", "time,rpm\n0,1000\n")

    with pytest.raises(CsvLoadError, match="different columns"):
        load_columns(csv_path, "time", "time")
