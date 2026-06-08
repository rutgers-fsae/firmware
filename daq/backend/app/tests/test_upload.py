from fastapi.testclient import TestClient

from app.config import settings
from app.main import app
from app.services.dataset_registry import registry


client = TestClient(app)


def test_upload_accepts_csv_with_password():
    response = client.post(
        "/api/upload",
        headers={"Authorization": f"Bearer {settings.upload_password}"},
        files={"file": ("test-upload.csv", b"a,b\n1,2\n", "text/csv")},
    )
    assert response.status_code == 200
    body = response.json()
    assert "slug" in body


def test_upload_rejects_files_over_limit(monkeypatch):
    monkeypatch.setattr(settings, "max_upload_bytes", 4)
    response = client.post(
        "/api/upload",
        headers={"Authorization": f"Bearer {settings.upload_password}"},
        files={"file": ("too-large.csv", b"a,b\n1,2\n", "text/csv")},
    )
    assert response.status_code == 413


def test_upload_same_filename_preserves_existing_file():
    first = client.post(
        "/api/upload",
        headers={"Authorization": f"Bearer {settings.upload_password}"},
        files={"file": ("duplicate.csv", b"a,b\n1,2\n", "text/csv")},
    )
    second = client.post(
        "/api/upload",
        headers={"Authorization": f"Bearer {settings.upload_password}"},
        files={"file": ("duplicate.csv", b"a,b\n3,4\n", "text/csv")},
    )

    assert first.status_code == 200
    assert second.status_code == 200
    assert first.json()["filename"] == "duplicate.csv"
    assert second.json()["filename"] == "duplicate-2.csv"
    assert (settings.data_dir / "duplicate.csv").read_text(encoding="utf-8") == "a,b\n1,2\n"
    assert (settings.data_dir / "duplicate-2.csv").read_text(encoding="utf-8") == "a,b\n3,4\n"
    records = registry.list()
    assert {item.original_name for item in records} == {"duplicate.csv"}
