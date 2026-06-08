from fastapi import APIRouter

from app.config import settings
from app.core.errors import bad_request
from app.models.chart import ChartRequest, PreviewRequest
from app.models.dataset import DatasetListItem
from app.services.chart_builder import build_chart_payload
from app.services.csv_reader import apply_filters, read_dataset, read_dataset_with_units
from app.services.dataset_registry import registry
from app.services.schema_inference import infer_schema

router = APIRouter(prefix="/api/datasets", tags=["datasets"])


@router.get("", response_model=list[DatasetListItem])
def list_datasets() -> list[DatasetListItem]:
    registry.ensure_initial_data_registered()
    return [
        DatasetListItem(
            slug=item.slug,
            title=item.title,
            filename=item.filename,
            uploaded_at=item.uploaded_at,
            size_bytes=item.size_bytes,
        )
        for item in registry.list()
    ]


@router.get("/{slug}/schema")
def get_dataset_schema(slug: str) -> dict:
    df, units = read_dataset_with_units(slug)
    return {"columns": infer_schema(df, units), "row_count": len(df)}


@router.post("/{slug}/preview")
def preview_dataset(slug: str, payload: PreviewRequest) -> dict:
    df = read_dataset(slug)
    filtered = apply_filters(df, [rule.model_dump() for rule in payload.filters])
    limit = payload.limit or settings.max_preview_rows
    records = filtered.head(limit).to_dict(orient="records")
    return {"rows": records, "row_count": len(filtered)}


@router.post("/{slug}/chart-data")
def chart_data(slug: str, payload: ChartRequest) -> dict:
    filters = [rule.model_dump() for rule in payload.filters]
    requested_columns = _columns_for_chart(payload, filters)
    df = read_dataset(slug, requested_columns)
    _validate_requested_columns(df.columns, requested_columns)
    filtered = apply_filters(df, filters)
    return build_chart_payload(filtered, payload.chart_type, payload.x_column, payload.y_columns, payload.group_by)


def _columns_for_chart(payload: ChartRequest, filters: list[dict]) -> set[str] | None:
    columns = set(payload.y_columns)
    if payload.x_column:
        columns.add(payload.x_column)
    if payload.group_by:
        columns.add(payload.group_by)
    for rule in filters:
        column = rule.get("column")
        if isinstance(column, str):
            columns.add(column)
    return columns or None


def _validate_requested_columns(available_columns, requested_columns: set[str] | None) -> None:
    if not requested_columns:
        return
    missing = sorted(requested_columns - set(available_columns))
    if missing:
        raise bad_request(f"Unknown dataset columns: {', '.join(missing)}")
