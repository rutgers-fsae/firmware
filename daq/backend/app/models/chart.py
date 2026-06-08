from typing import Any
from typing import Literal

from pydantic import BaseModel, Field, field_validator

from app.config import settings


FilterOp = Literal["eq", "contains", "gte", "lte"]
ChartType = Literal["line", "scatter", "bar", "histogram", "box"]


class FilterRule(BaseModel):
    column: str
    op: FilterOp
    value: Any


class ChartRequest(BaseModel):
    chart_type: ChartType
    x_column: str | None = None
    y_columns: list[str] = Field(default_factory=list, min_length=1)
    group_by: str | None = None
    filters: list[FilterRule] = Field(default_factory=list)


class PreviewRequest(BaseModel):
    filters: list[FilterRule] = Field(default_factory=list)
    limit: int | None = Field(default=None, ge=1)

    @field_validator("limit")
    @classmethod
    def limit_cannot_exceed_max_preview_rows(cls, value: int | None) -> int | None:
        if value is not None and value > settings.max_preview_rows:
            raise ValueError(f"limit cannot exceed {settings.max_preview_rows}")
        return value
