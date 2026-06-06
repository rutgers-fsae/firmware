from pathlib import Path

from fastapi import APIRouter, Depends, File, UploadFile

from app.config import settings
from app.core.errors import bad_request, payload_too_large
from app.core.auth import require_upload_password
from app.services.dataset_registry import registry

router = APIRouter(prefix="/api/upload", tags=["upload"])


@router.post("")
async def upload_csv(_: None = Depends(require_upload_password), file: UploadFile = File(...)) -> dict:
    if not file.filename or not file.filename.lower().endswith(".csv"):
        raise bad_request("Only .csv files are supported")

    destination = settings.data_dir / Path(file.filename).name
    destination.parent.mkdir(parents=True, exist_ok=True)
    size_bytes = 0
    with destination.open("wb") as output:
        while chunk := await file.read(settings.upload_chunk_bytes):
            size_bytes += len(chunk)
            if size_bytes > settings.max_upload_bytes:
                output.close()
                destination.unlink(missing_ok=True)
                raise payload_too_large("CSV exceeds the configured upload size limit")
            output.write(chunk)

    record = registry.register(destination.name, size_bytes)
    return {"slug": record.slug, "title": record.title, "filename": record.filename}
