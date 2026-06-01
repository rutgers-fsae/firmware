import json
from datetime import datetime, timezone
from pathlib import Path

from app.config import settings
from app.core.errors import not_found
from app.core.slug import slugify, unique_slug
from app.models.dataset import DatasetRecord


class DatasetRegistry:
    def __init__(self, path: Path):
        self.path = path
        self.path.parent.mkdir(parents=True, exist_ok=True)
        if not self.path.exists():
            self.path.write_text("[]", encoding="utf-8")

    def _read(self) -> list[DatasetRecord]:
        payload = json.loads(self.path.read_text(encoding="utf-8"))
        return [DatasetRecord.model_validate(item) for item in payload]

    def _write(self, records: list[DatasetRecord]) -> None:
        self.path.write_text(
            json.dumps([item.model_dump(mode="json") for item in records], indent=2),
            encoding="utf-8",
        )

    def list(self) -> list[DatasetRecord]:
        return sorted(self._read(), key=lambda x: x.uploaded_at, reverse=True)

    def get(self, slug: str) -> DatasetRecord:
        for record in self._read():
            if record.slug == slug:
                return record
        raise not_found("Dataset not found")

    def register(self, filename: str, size_bytes: int) -> DatasetRecord:
        records = self._read()
        existing = {item.slug for item in records}
        stem = Path(filename).stem
        slug = unique_slug(slugify(stem), existing)
        record = DatasetRecord(
            slug=slug,
            filename=filename,
            original_name=filename,
            title=stem,
            uploaded_at=datetime.now(timezone.utc),
            size_bytes=size_bytes,
        )
        records.append(record)
        self._write(records)
        return record

    def ensure_initial_data_registered(self) -> None:
        records = self._read()
        existing_files = {item.filename for item in records}
        for path in settings.data_dir.glob("*.csv"):
            if path.name not in existing_files:
                self.register(path.name, path.stat().st_size)


registry = DatasetRegistry(settings.registry_path)
