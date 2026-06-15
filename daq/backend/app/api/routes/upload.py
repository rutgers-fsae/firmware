import logging
from pathlib import Path

from fastapi import APIRouter, Depends, File, UploadFile

from app.config import settings
from app.core.errors import bad_request, payload_too_large
from app.core.auth import require_upload_password
from app.core.slug import slugify
from app.services.dataset_registry import registry
from app.services.csv_reader import ensure_parquet_sidecar

router = APIRouter(prefix="/api/upload", tags=["upload"])
logger = logging.getLogger(__name__)


@router.post("")
async def upload_csv(_: None = Depends(require_upload_password), file: UploadFile = File(...)) -> dict:
    if not file.filename or not file.filename.lower().endswith(".csv"):
        raise bad_request("Only .csv files are supported")

    original_name = Path(file.filename).name
    destination, output = _create_unique_upload_file(original_name)
    size_bytes = 0
    with output:
        while chunk := await file.read(settings.upload_chunk_bytes):
            size_bytes += len(chunk)
            if size_bytes > settings.max_upload_bytes:
                output.close()
                destination.unlink(missing_ok=True)
                raise payload_too_large("CSV exceeds the configured upload size limit")
            output.write(chunk)

    record = registry.register(destination.name, size_bytes, original_name=original_name)
    try:
        ensure_parquet_sidecar(destination)
    except Exception as exc:
        logger.warning("Unable to create parquet sidecar for %s: %s", destination, exc)
    return {"slug": record.slug, "title": record.title, "filename": record.filename}


def _create_unique_upload_file(filename: str):
    original = Path(filename)
    base = slugify(original.stem) or "dataset"
    suffix = original.suffix.lower()
    counter = 2
    destination = settings.data_dir / f"{base}{suffix}"
    destination.parent.mkdir(parents=True, exist_ok=True)
    while True:
        try:
            return destination, destination.open("xb")
        except FileExistsError:
            pass
        destination = settings.data_dir / f"{base}-{counter}{suffix}"
        counter += 1
