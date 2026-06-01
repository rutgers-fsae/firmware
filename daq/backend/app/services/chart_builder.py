from typing import Any

from app.config import settings


def build_chart_payload(df, chart_type: str, x_column: str | None, y_columns: list[str], group_by: str | None) -> dict[str, Any]:
    if len(df) > settings.max_chart_rows:
        df = df.head(settings.max_chart_rows)

    data = []
    if group_by and group_by in df.columns:
        for group, group_df in df.groupby(group_by):
            for y in y_columns:
                if y in group_df.columns:
                    data.append(
                        {
                            "name": f"{group} - {y}",
                            "x": group_df[x_column].tolist() if x_column else list(range(len(group_df))),
                            "y": group_df[y].tolist(),
                            "type": chart_type,
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
                        "type": chart_type,
                    }
                )

    return {"data": data, "row_count": len(df)}
