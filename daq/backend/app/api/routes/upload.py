from pathlib import Path

from fastapi import APIRouter, Depends, File, UploadFile

from app.config import settings
from app.core.errors import bad_request
from app.core.auth import require_upload_password
from app.services.dataset_registry import registry

router = APIRouter(prefix="/api/upload", tags=["upload"])


@router.post("")
async def upload_csv(_: None = Depends(require_upload_password), file: UploadFile = File(...)) -> dict:
    if not file.filename or not file.filename.lower().endswith(".csv"):
        raise bad_request("Only .csv files are supported")

    destination = settings.data_dir / Path(file.filename).name
    content = await file.read()
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(content)

    record = registry.register(destination.name, len(content))
    return {"slug": record.slug, "title": record.title, "filename": record.filename}
