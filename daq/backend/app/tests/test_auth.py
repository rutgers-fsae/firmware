from fastapi.testclient import TestClient

from app.main import app


client = TestClient(app)


def test_upload_auth_rejected_without_token():
    response = client.post("/api/upload")
    assert response.status_code == 401
