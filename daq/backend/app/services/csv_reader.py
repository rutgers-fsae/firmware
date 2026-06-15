import csv
from functools import lru_cache
from pathlib import Path
import re
from typing import Iterable

import pandas as pd
from pandas.errors import ParserError

from app.config import settings
from app.core.errors import bad_request
from app.services.dataset_registry import registry


def dataset_path_for_slug(slug: str) -> Path:
    record = registry.get(slug)
    path = (settings.data_dir / record.filename).resolve()
    if settings.data_dir.resolve() not in path.parents:
        raise bad_request("Invalid dataset path")
    if not path.exists():
        raise bad_request("Dataset file is missing")
    return path


def read_dataset(slug: str, columns: Iterable[str] | None = None) -> pd.DataFrame:
    frame, _ = read_dataset_with_units(slug, columns)
    return frame


def read_dataset_with_units(slug: str, columns: Iterable[str] | None = None) -> tuple[pd.DataFrame, dict[str, str | None]]:
    path = dataset_path_for_slug(slug)
    stat = path.stat()
    columns_key = tuple(sorted(set(columns))) if columns else None
    frame, units = _read_dataset_cached(str(path), stat.st_mtime_ns, stat.st_size, columns_key)
    return frame.copy(deep=False), dict(units)


def read_dataset_sample_with_units(slug: str, sample_rows: int = 100) -> tuple[pd.DataFrame, dict[str, str | None], int]:
    path = dataset_path_for_slug(slug)
    if _is_motec_csv(path):
        frame, units = _read_motec_csv(path, nrows=sample_rows)
    else:
        frame = _read_regular_csv(path, nrows=sample_rows)
        units = _infer_units_from_column_names(frame.columns.tolist())
    return frame, units, _row_count_for_path(path)


def read_dataset_preview(slug: str, limit: int) -> tuple[pd.DataFrame, int]:
    path = dataset_path_for_slug(slug)
    if _is_motec_csv(path):
        frame, _ = _read_motec_csv(path, nrows=limit)
    else:
        frame = _read_regular_csv(path, nrows=limit)
    return frame, _row_count_for_path(path)


@lru_cache(maxsize=3)
def _read_dataset_cached(
    path_value: str,
    _mtime_ns: int,
    _size_bytes: int,
    columns_key: tuple[str, ...] | None,
) -> tuple[pd.DataFrame, dict[str, str | None]]:
    path = Path(path_value)
    columns = set(columns_key) if columns_key else None
    parquet_frame = _read_parquet_sidecar(path, columns)
    if parquet_frame is not None:
        return parquet_frame, _units_for_path(path, parquet_frame.columns.tolist())

    if _is_motec_csv(path):
        frame, units = _read_motec_csv(path, columns)
    else:
        frame = _read_regular_csv(path, columns)
        units = _infer_units_from_column_names(frame.columns.tolist())

    if columns is None:
        ensure_parquet_sidecar(path, frame)
    return frame, units


def _read_regular_csv(path: Path, columns: set[str] | None = None, nrows: int | None = None) -> pd.DataFrame:
    kwargs = _read_csv_kwargs(columns, nrows)
    try:
        return pd.read_csv(path, **kwargs)
    except ParserError:
        try:
            return pd.read_csv(path, engine="python", on_bad_lines="skip", **kwargs)
        except Exception as exc:
            raise bad_request(f"Unable to parse CSV: {exc}") from exc


def _read_motec_csv(
    path: Path,
    columns: set[str] | None = None,
    nrows: int | None = None,
) -> tuple[pd.DataFrame, dict[str, str | None]]:
    header_index, header_row, units_row = _find_motec_header(path)
    units_row_index = header_index + 1
    skiprows = [*range(header_index), units_row_index]
    kwargs = _read_csv_kwargs(columns, nrows)
    try:
        frame = pd.read_csv(path, skiprows=skiprows, **kwargs)
    except ParserError:
        try:
            frame = pd.read_csv(path, skiprows=skiprows, engine="python", on_bad_lines="skip", **kwargs)
        except Exception as exc:
            raise bad_request(f"Unable to parse CSV: {exc}") from exc
    except Exception as exc:
        raise bad_request(f"Unable to parse CSV: {exc}") from exc

    frame = frame.dropna(how="all")
    if not frame.empty and "Time" in frame.columns:
        numeric_time = pd.to_numeric(frame["Time"], errors="coerce")
        frame = frame[numeric_time.notna()].copy()

    for col in frame.columns:
        converted = pd.to_numeric(frame[col], errors="coerce")
        if converted.notna().any():
            frame[col] = converted

    clean = frame.reset_index(drop=True)
    units = _units_map_for_columns(clean.columns.tolist(), header_row, units_row)
    return clean, units


def _read_csv_kwargs(columns: set[str] | None, nrows: int | None = None) -> dict:
    kwargs = {}
    if columns:
        kwargs["usecols"] = lambda column: column in columns
    if nrows is not None:
        kwargs["nrows"] = nrows
    return kwargs


def _is_motec_csv(path: Path) -> bool:
    with path.open("r", encoding="utf-8", newline="") as handle:
        first_row = next(csv.reader(handle), [])
    return bool(first_row and first_row[0].strip().strip('"') == "Format")


def _find_motec_header(path: Path) -> tuple[int, list[str], list[str]]:
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.reader(handle)
        for index, row in enumerate(reader):
            if row and row[0].strip().strip('"') == "Time":
                return index, row, next(reader, [])

    raise bad_request("Unable to parse CSV: no data header row found")


def _units_for_path(path: Path, columns: list[str]) -> dict[str, str | None]:
    if _is_motec_csv(path):
        _, header_row, units_row = _find_motec_header(path)
        return _units_map_for_columns(columns, header_row, units_row)
    return _infer_units_from_column_names(columns)


def _parquet_sidecar_path(path: Path) -> Path:
    return path.with_suffix(f"{path.suffix}.parquet")


def _read_parquet_sidecar(path: Path, columns: set[str] | None = None) -> pd.DataFrame | None:
    sidecar = _parquet_sidecar_path(path)
    if not sidecar.exists() or sidecar.stat().st_mtime_ns < path.stat().st_mtime_ns:
        return None

    try:
        return pd.read_parquet(sidecar, columns=list(columns) if columns else None)
    except (ImportError, ValueError, FileNotFoundError, OSError):
        return None


def ensure_parquet_sidecar(path: Path, frame: pd.DataFrame | None = None) -> None:
    sidecar = _parquet_sidecar_path(path)
    if sidecar.exists() and sidecar.stat().st_mtime_ns >= path.stat().st_mtime_ns:
        return
    if frame is None:
        if _is_motec_csv(path):
            frame, _ = _read_motec_csv(path)
        else:
            frame = _read_regular_csv(path)
    _write_parquet_sidecar(path, frame)


def _write_parquet_sidecar(path: Path, frame: pd.DataFrame) -> None:
    sidecar = _parquet_sidecar_path(path)
    try:
        frame.to_parquet(sidecar, index=False)
    except (ImportError, ValueError, OSError):
        sidecar.unlink(missing_ok=True)


def _row_count_for_path(path: Path) -> int:
    sidecar = _parquet_sidecar_path(path)
    if sidecar.exists() and sidecar.stat().st_mtime_ns >= path.stat().st_mtime_ns:
        try:
            return int(pd.read_parquet(sidecar, columns=[]).shape[0])
        except (ImportError, ValueError, FileNotFoundError, OSError):
            pass

    if _is_motec_csv(path):
        frame, _ = _read_motec_csv(path, {"Time"})
        return len(frame)

    try:
        first_column = pd.read_csv(path, nrows=0).columns[:1].tolist()
        if not first_column:
            return 0
        count = 0
        for chunk in pd.read_csv(path, usecols=first_column, chunksize=100_000):
            count += len(chunk)
        return count
    except Exception as exc:
        raise bad_request(f"Unable to parse CSV: {exc}") from exc


def _units_map_for_columns(columns: list[str], header_row: list[str], raw_units: list[str]) -> dict[str, str | None]:
    raw_units_by_column = {
        column: raw_units[index].strip().strip('"') if index < len(raw_units) else ""
        for index, column in enumerate(header_row)
    }
    units: dict[str, str | None] = {}
    for column in columns:
        units[column] = raw_units_by_column.get(column) or None
    return units


def _infer_units_from_column_names(columns: list[str]) -> dict[str, str | None]:
    units: dict[str, str | None] = {}
    pattern = re.compile(r"\(([^)]+)\)\s*$")
    for column in columns:
        match = pattern.search(column)
        units[column] = match.group(1).strip() if match else None
    return units


def apply_filters(df: pd.DataFrame, filters: list[dict]) -> pd.DataFrame:
    result = df
    for rule in filters:
        col = rule.get("column")
        op = rule.get("op")
        value = rule.get("value")
        if col not in result.columns:
            continue
        if op == "eq":
            result = result[result[col] == value]
        elif op == "contains":
            result = result[result[col].astype(str).str.contains(str(value), na=False)]
        elif op == "gte":
            result = result[result[col] >= value]
        elif op == "lte":
            result = result[result[col] <= value]
    return result
