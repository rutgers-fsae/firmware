from typing import Any

import numpy as np

from app.config import settings


def _plotly_trace_options(chart_type: str) -> dict[str, str]:
    if chart_type == "line":
        return {"type": "scatter", "mode": "lines"}
    if chart_type == "scatter":
        return {"type": "scatter", "mode": "markers"}
    return {"type": chart_type}


def build_chart_payload(df, chart_type: str, x_column: str | None, y_columns: list[str], group_by: str | None) -> dict[str, Any]:
    source_row_count = len(df)
    was_downsampled = source_row_count > settings.max_chart_rows
    if was_downsampled:
        indices = np.linspace(0, source_row_count - 1, settings.max_chart_rows, dtype=int)
        df = df.iloc[np.unique(indices)].reset_index(drop=True)

    data = []
    trace_options = _plotly_trace_options(chart_type)
    if group_by and group_by in df.columns:
        for group, group_df in df.groupby(group_by):
            for y in y_columns:
                if y in group_df.columns:
                    data.append(
                        {
                            "name": f"{group} - {y}",
                            "x": group_df[x_column].tolist() if x_column else list(range(len(group_df))),
                            "y": group_df[y].tolist(),
                            **trace_options,
                        }
                    )
    else:
        for y in y_columns:
            if y in df.columns:
                data.append(
                    {
                        "name": y,
                        "x": df[x_column].tolist() if x_column else list(range(len(df))),
                        "y": df[y].tolist(),
                        **trace_options,
                    }
                )

    return {
        "data": data,
        "row_count": len(df),
        "source_row_count": source_row_count,
        "returned_row_count": len(df),
        "was_downsampled": was_downsampled,
    }
