from typing import Any

import pandas as pd


def infer_type(series: pd.Series) -> str:
    if pd.api.types.is_numeric_dtype(series):
        return "numeric"
    if pd.api.types.is_datetime64_any_dtype(series):
        return "datetime"
    return "categorical"


def infer_schema(df: pd.DataFrame, units_by_column: dict[str, str | None] | None = None) -> list[dict[str, Any]]:
    schema = []
    units_by_column = units_by_column or {}
    for col in df.columns:
        sample_values = df[col].dropna().astype(str).head(5).tolist()
        unit = units_by_column.get(col)
        display_name = f"{col} ({unit})" if unit else col
        schema.append(
            {
                "name": col,
                "type": infer_type(df[col]),
                "unit": unit,
                "display_name": display_name,
                "sample_values": sample_values,
            }
        )
    return schema
