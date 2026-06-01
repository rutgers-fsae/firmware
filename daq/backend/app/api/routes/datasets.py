from fastapi import APIRouter

from app.config import settings
from app.models.chart import ChartRequest, PreviewRequest
from app.models.dataset import DatasetListItem
from app.services.chart_builder import build_chart_payload
from app.services.csv_reader import apply_filters, read_dataset
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
    df = read_dataset(slug)
    return {"columns": infer_schema(df), "row_count": len(df)}


@router.post("/{slug}/preview")
def preview_dataset(slug: str, payload: PreviewRequest) -> dict:
    df = read_dataset(slug)
    filtered = apply_filters(df, [rule.model_dump() for rule in payload.filters])
    limit = min(payload.limit or settings.max_preview_rows, settings.max_preview_rows)
    records = filtered.head(limit).to_dict(orient="records")
    return {"rows": records, "row_count": len(filtered)}


@router.post("/{slug}/chart-data")
def chart_data(slug: str, payload: ChartRequest) -> dict:
    df = read_dataset(slug)
    filtered = apply_filters(df, [rule.model_dump() for rule in payload.filters])
    return build_chart_payload(filtered, payload.chart_type, payload.x_column, payload.y_columns, payload.group_by)
