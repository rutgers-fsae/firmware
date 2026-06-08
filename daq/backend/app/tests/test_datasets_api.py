from fastapi.testclient import TestClient

from app.config import settings
from app.main import app
from app.services.dataset_registry import registry
from app.services.csv_reader import _read_motec_csv


client = TestClient(app)


def test_datasets_list_works():
    response = client.get("/api/datasets")
    assert response.status_code == 200
    assert isinstance(response.json(), list)


def test_motec_reader_supports_selected_columns(tmp_path):
    csv_path = tmp_path / "motec.csv"
    csv_path.write_text(
        '\n'.join(
            [
                '"Format","MoTeC CSV File"',
                '"Sample Rate","500.000","Hz"',
                '',
                '"Time","Battery Temp","Motor Speed"',
                '"s","C","rpm"',
                '',
                '"0.000","18.0","0"',
                '"0.002","18.1","10"',
            ]
        ),
        encoding="utf-8",
    )

    frame, units = _read_motec_csv(csv_path, {"Time", "Motor Speed"})

    assert frame.columns.tolist() == ["Time", "Motor Speed"]
    assert frame.to_dict(orient="records") == [
        {"Time": 0.0, "Motor Speed": 0},
        {"Time": 0.002, "Motor Speed": 10},
    ]
    assert units == {"Time": "s", "Motor Speed": "rpm"}


def test_chart_data_rejects_missing_columns():
    csv_path = settings.data_dir / "sample.csv"
    csv_path.write_text("Time,Speed\n0,10\n1,20\n", encoding="utf-8")
    record = registry.register(csv_path.name, csv_path.stat().st_size)

    response = client.post(
        f"/api/datasets/{record.slug}/chart-data",
        json={"chart_type": "line", "x_column": "Time", "y_columns": ["Missing"], "filters": []},
    )

    assert response.status_code == 400
    assert response.json()["detail"] == "Unknown dataset columns: Missing"


def test_preview_rejects_limit_above_configured_max():
    csv_path = settings.data_dir / "sample.csv"
    csv_path.write_text("Time,Speed\n0,10\n1,20\n", encoding="utf-8")
    record = registry.register(csv_path.name, csv_path.stat().st_size)

    response = client.post(
        f"/api/datasets/{record.slug}/preview",
        json={"filters": [], "limit": settings.max_preview_rows + 1},
    )

    assert response.status_code == 422
