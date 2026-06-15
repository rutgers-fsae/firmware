from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import numpy as np
import polars as pl


@dataclass(frozen=True)
class CsvSchema:
    path: Path
    columns: list[str]


@dataclass(frozen=True)
class CsvHeader:
    skip_rows: int
    columns: list[str] | None


@dataclass(frozen=True)
class LoadedColumns:
    x: np.ndarray
    y: np.ndarray
    x_label: str
    y_label: str
    total_rows: int
    valid_rows: int
    dropped_rows: int
    x_is_datetime: bool


class CsvLoadError(RuntimeError):
    pass


def read_schema(path: str | Path) -> CsvSchema:
    csv_path = Path(path)
    if not csv_path.exists():
        raise CsvLoadError(f"File does not exist: {csv_path}")
    if not csv_path.is_file():
        raise CsvLoadError(f"Path is not a file: {csv_path}")

    header = _inspect_header(csv_path)
    if header.columns is not None:
        columns = header.columns
    else:
        try:
            lazy_frame = _scan_csv(csv_path, skip_rows=header.skip_rows)
            columns = list(lazy_frame.collect_schema().names())
        except Exception as exc:  # Polars exceptions vary by parser failure.
            raise CsvLoadError(f"Could not read CSV headers: {exc}") from exc

    if not columns:
        raise CsvLoadError("CSV does not contain any columns")
    return CsvSchema(path=csv_path, columns=columns)


def load_columns(path: str | Path, x_column: str, y_column: str) -> LoadedColumns:
    csv_path = Path(path)
    if x_column == y_column:
        raise CsvLoadError("Choose two different columns")

    header = _inspect_header(csv_path)
    try:
        lazy_frame = _scan_csv(csv_path, skip_rows=header.skip_rows).select([pl.col(x_column), pl.col(y_column)])
    except Exception as exc:
        raise CsvLoadError(f"Could not prepare CSV scan: {exc}") from exc

    try:
        frame = _collect(lazy_frame)
    except Exception as exc:
        raise CsvLoadError(f"Could not load selected columns: {exc}") from exc

    total_rows = frame.height
    if total_rows == 0:
        raise CsvLoadError("Selected columns contain no rows")

    y_expr = _numeric_expr(y_column)
    x_dtype = frame.schema[x_column]
    x_is_datetime = _looks_datetime(frame[x_column], x_dtype)
    x_expr = _native_datetime_expr(x_column) if _is_datetime_dtype(x_dtype) else (
        _datetime_expr(x_column) if x_is_datetime else _numeric_expr(x_column)
    )

    try:
        clean = (
            frame.with_columns(
                [
                    x_expr.alias("__x__"),
                    y_expr.alias("__y__"),
                ]
            )
            .select(["__x__", "__y__"])
            .drop_nulls()
            .filter(pl.col("__x__").is_finite() & pl.col("__y__").is_finite())
        )
    except Exception as exc:
        raise CsvLoadError(f"Could not convert selected columns to plottable values: {exc}") from exc

    valid_rows = clean.height
    if valid_rows == 0:
        raise CsvLoadError("No numeric/datetime rows were found for the selected columns")

    x = clean["__x__"].to_numpy()
    y = clean["__y__"].to_numpy()
    return LoadedColumns(
        x=x,
        y=y,
        x_label=x_column,
        y_label=y_column,
        total_rows=total_rows,
        valid_rows=valid_rows,
        dropped_rows=total_rows - valid_rows,
        x_is_datetime=x_is_datetime,
    )


def _collect(lazy_frame: pl.LazyFrame) -> pl.DataFrame:
    try:
        return lazy_frame.collect(engine="streaming")
    except TypeError:
        try:
            return lazy_frame.collect(streaming=True)
        except TypeError:
            return lazy_frame.collect()


def _scan_csv(path: Path, skip_rows: int) -> pl.LazyFrame:
    return pl.scan_csv(
        path,
        skip_rows=skip_rows,
        infer_schema_length=1000,
        ignore_errors=True,
        truncate_ragged_lines=True,
    )


def _inspect_header(path: Path, max_lines: int = 200) -> CsvHeader:
    try:
        with path.open("r", encoding="utf-8-sig", newline="") as file:
            rows = list(csv.reader(_read_limited_lines(file, max_lines)))
    except UnicodeDecodeError:
        with path.open("r", encoding="latin-1", newline="") as file:
            rows = list(csv.reader(_read_limited_lines(file, max_lines)))
    except OSError as exc:
        raise CsvLoadError(f"Could not inspect CSV header: {exc}") from exc

    if not rows:
        return CsvHeader(skip_rows=0, columns=None)

    first_row = [cell.strip().lower() for cell in rows[0]]
    is_motec = len(first_row) >= 2 and first_row[0] == "format" and "motec csv file" in first_row[1]
    if not is_motec:
        return CsvHeader(skip_rows=0, columns=None)

    for index, row in enumerate(rows):
        cells = [cell.strip() for cell in row]
        non_empty = [cell for cell in cells if cell]
        if len(non_empty) >= 2 and cells and cells[0].lower() == "time":
            return CsvHeader(skip_rows=index, columns=_dedupe_columns(cells))

    return CsvHeader(skip_rows=0, columns=None)


def _dedupe_columns(columns: list[str]) -> list[str]:
    counts: dict[str, int] = {}
    deduped: list[str] = []
    for index, column in enumerate(columns):
        base_name = column or f"column_{index + 1}"
        count = counts.get(base_name, 0)
        counts[base_name] = count + 1
        deduped.append(base_name if count == 0 else f"{base_name}_{count}")
    return deduped


def _read_limited_lines(file: Any, max_lines: int) -> list[str]:
    lines: list[str] = []
    for _, line in zip(range(max_lines), file):
        lines.append(line)
    return lines


def _numeric_expr(column: str) -> pl.Expr:
    return pl.col(column).cast(pl.Float64, strict=False)


def _datetime_expr(column: str) -> pl.Expr:
    return (
        pl.col(column)
        .str.strptime(pl.Datetime, strict=False)
        .dt.timestamp("ms")
        .truediv(1000.0)
        .cast(pl.Float64, strict=False)
    )


def _native_datetime_expr(column: str) -> pl.Expr:
    return (
        pl.col(column)
        .cast(pl.Datetime)
        .dt.timestamp("ms")
        .truediv(1000.0)
        .cast(pl.Float64, strict=False)
    )


def _is_datetime_dtype(dtype: Any) -> bool:
    return dtype in {pl.Date, pl.Datetime, pl.Time}


def _looks_datetime(series: pl.Series, dtype: Any) -> bool:
    if _is_datetime_dtype(dtype):
        return True
    if dtype != pl.String:
        return False

    sample = series.drop_nulls().head(50)
    if sample.is_empty():
        return False

    try:
        parsed = sample.str.strptime(pl.Datetime, strict=False)
    except Exception:
        return False
    return parsed.drop_nulls().len() >= max(1, sample.len() // 2)
