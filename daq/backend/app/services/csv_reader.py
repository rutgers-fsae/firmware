from pathlib import Path
import csv

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


def read_dataset(slug: str) -> pd.DataFrame:
    path = dataset_path_for_slug(slug)
    try:
        return pd.read_csv(path)
    except ParserError:
        try:
            frame = pd.read_csv(path, engine="python", on_bad_lines="skip")
            if frame.columns[0] == "Format":
                return _read_motec_csv(path)
            return frame
        except Exception as exc:
            raise bad_request(f"Unable to parse CSV: {exc}") from exc


def _read_motec_csv(path: Path) -> pd.DataFrame:
    with path.open("r", encoding="utf-8", newline="") as handle:
        rows = list(csv.reader(handle))

    header_index = None
    for index, row in enumerate(rows):
        if row and row[0].strip().strip('"') == "Time":
            header_index = index
            break

    if header_index is None:
        raise bad_request("Unable to parse CSV: no data header row found")

    frame = pd.read_csv(path, skiprows=header_index, engine="python", on_bad_lines="skip")
    frame = frame.dropna(how="all")
    if not frame.empty:
        time_col = frame.columns[0]
        numeric_time = pd.to_numeric(frame[time_col], errors="coerce")
        frame = frame[numeric_time.notna()].copy()

    for col in frame.columns:
        converted = pd.to_numeric(frame[col], errors="coerce")
        if converted.notna().any():
            frame[col] = converted

    return frame.reset_index(drop=True)


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
