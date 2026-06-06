from fastapi.testclient import TestClient

from app.config import settings
from app.main import app


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
