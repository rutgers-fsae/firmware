from __future__ import annotations

from pathlib import Path

from offline_daq_viewer.data.paths import default_csv_dir, resolve_csv_inputs


def test_resolve_csv_inputs_accepts_relative_glob(tmp_path: Path) -> None:
    workspace = tmp_path / "offline-daq"
    data_dir = tmp_path / "daq" / "data"
    workspace.mkdir()
    data_dir.mkdir(parents=True)
    first = data_dir / "a.csv"
    second = data_dir / "b.csv"
    ignored = data_dir / "notes.txt"
    first.write_text("x,y\n1,2\n", encoding="utf-8")
    second.write_text("x,y\n3,4\n", encoding="utf-8")
    ignored.write_text("not csv", encoding="utf-8")

    paths = resolve_csv_inputs(["../daq/data/*.csv"], cwd=workspace)

    assert paths == [first.resolve(), second.resolve()]


def test_default_csv_dir_prefers_relative_daq_data(tmp_path: Path) -> None:
    workspace = tmp_path / "offline-daq"
    data_dir = tmp_path / "daq" / "data"
    workspace.mkdir()
    data_dir.mkdir(parents=True)

    assert default_csv_dir(workspace) == data_dir.resolve()

