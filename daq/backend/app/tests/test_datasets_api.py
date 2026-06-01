from fastapi.testclient import TestClient

from app.main import app


client = TestClient(app)


def test_datasets_list_works():
    response = client.get("/api/datasets")
    assert response.status_code == 200
    assert isinstance(response.json(), list)
