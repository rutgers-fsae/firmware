import pytest

from app.config import settings
from app.services.dataset_registry import registry


@pytest.fixture(autouse=True)
def isolated_dataset_storage(tmp_path, monkeypatch):
    data_dir = tmp_path / "data"
    registry_path = tmp_path / "storage" / "datasets.json"
    data_dir.mkdir()
    registry_path.parent.mkdir()
    registry_path.write_text("[]", encoding="utf-8")

    monkeypatch.setattr(settings, "data_dir", data_dir)
    monkeypatch.setattr(settings, "registry_path", registry_path)
    monkeypatch.setattr(registry, "path", registry_path)
