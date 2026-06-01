from typing import Any

import pandas as pd


def infer_type(series: pd.Series) -> str:
    if pd.api.types.is_numeric_dtype(series):
        return "numeric"
    if pd.api.types.is_datetime64_any_dtype(series):
        return "datetime"
    return "categorical"


def infer_schema(df: pd.DataFrame) -> list[dict[str, Any]]:
    schema = []
    for col in df.columns:
        sample_values = df[col].dropna().astype(str).head(5).tolist()
        schema.append(
            {
                "name": col,
                "type": infer_type(df[col]),
                "sample_values": sample_values,
            }
        )
    return schema
