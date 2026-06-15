from __future__ import annotations

from pathlib import Path


DEFAULT_DAQ_DATA_DIR = Path("../daq/data")


def default_csv_dir(cwd: Path | None = None) -> Path:
    base = cwd or Path.cwd()
    candidate = (base / DEFAULT_DAQ_DATA_DIR).resolve()
    return candidate if candidate.is_dir() else base


def resolve_csv_inputs(inputs: list[str], cwd: Path | None = None) -> list[Path]:
    base = cwd or Path.cwd()
    paths: list[Path] = []

    for raw_input in inputs:
        raw_path = Path(raw_input).expanduser()
        search_path = raw_path if raw_path.is_absolute() else base / raw_path

        if _has_glob(raw_input):
            matches = sorted(path.resolve() for path in search_path.parent.glob(search_path.name) if path.is_file())
            paths.extend(matches)
        elif search_path.is_file():
            paths.append(search_path.resolve())

    return _dedupe(paths)


def _has_glob(value: str) -> bool:
    return any(marker in value for marker in ("*", "?", "["))


def _dedupe(paths: list[Path]) -> list[Path]:
    seen: set[Path] = set()
    unique: list[Path] = []
    for path in paths:
        if path in seen:
            continue
        seen.add(path)
        unique.append(path)
    return unique

