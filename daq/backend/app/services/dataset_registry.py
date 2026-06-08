import json
import os
import threading
from datetime import datetime, timezone
from pathlib import Path

from app.config import settings
from app.core.errors import not_found
from app.core.slug import slugify, unique_slug
from app.models.dataset import DatasetRecord


class DatasetRegistry:
    def __init__(self, path: Path):
        self.path = path
        self._lock = threading.Lock()
        self.path.parent.mkdir(parents=True, exist_ok=True)
        if not self.path.exists():
            self.path.write_text("[]", encoding="utf-8")

    def _read(self) -> list[DatasetRecord]:
        payload = json.loads(self.path.read_text(encoding="utf-8"))
        return [DatasetRecord.model_validate(item) for item in payload]

    def _write(self, records: list[DatasetRecord]) -> None:
        payload = json.dumps([item.model_dump(mode="json") for item in records], indent=2)
        tmp_path = self.path.with_suffix(f"{self.path.suffix}.{os.getpid()}.{threading.get_ident()}.tmp")
        tmp_path.write_text(payload, encoding="utf-8")
        os.replace(tmp_path, self.path)

    def list(self) -> list[DatasetRecord]:
        with self._lock:
            return sorted(self._read(), key=lambda x: x.uploaded_at, reverse=True)

    def get(self, slug: str) -> DatasetRecord:
        with self._lock:
            for record in self._read():
                if record.slug == slug:
                    return record
        raise not_found("Dataset not found")

    def register(self, filename: str, size_bytes: int, original_name: str | None = None) -> DatasetRecord:
        with self._lock:
            records = self._read()
            existing = {item.slug for item in records}
            title = Path(original_name or filename).stem
            slug = unique_slug(slugify(title), existing)
            record = DatasetRecord(
                slug=slug,
                filename=filename,
                original_name=original_name or filename,
                title=title,
                uploaded_at=datetime.now(timezone.utc),
                size_bytes=size_bytes,
            )
            records.append(record)
            self._write(records)
            return record

    def ensure_initial_data_registered(self) -> None:
        with self._lock:
            records = self._read()
            existing_files = {item.filename for item in records}
            for path in settings.data_dir.glob("*.csv"):
                if path.name not in existing_files:
                    title = path.stem
                    existing = {item.slug for item in records}
                    record = DatasetRecord(
                        slug=unique_slug(slugify(title), existing),
                        filename=path.name,
                        original_name=path.name,
                        title=title,
                        uploaded_at=datetime.now(timezone.utc),
                        size_bytes=path.stat().st_size,
                    )
                    records.append(record)
                    existing_files.add(path.name)
            self._write(records)


registry = DatasetRegistry(settings.registry_path)
