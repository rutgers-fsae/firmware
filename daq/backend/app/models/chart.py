from typing import Any

from pydantic import BaseModel, Field


class FilterRule(BaseModel):
    column: str
    op: str
    value: Any


class ChartRequest(BaseModel):
    chart_type: str = Field(pattern="^(line|scatter|bar|histogram|box)$")
    x_column: str | None = None
    y_columns: list[str] = []
    group_by: str | None = None
    filters: list[FilterRule] = []


class PreviewRequest(BaseModel):
    filters: list[FilterRule] = []
    limit: int | None = None
