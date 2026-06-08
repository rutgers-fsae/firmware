import pandas as pd

from app.services.chart_builder import build_chart_payload


def test_line_chart_uses_plotly_scatter_lines():
    frame = pd.DataFrame({"Time": [0, 1], "Speed": [10, 20]})

    payload = build_chart_payload(frame, "line", "Time", ["Speed"], None)

    assert payload["data"] == [
        {
            "name": "Speed",
            "x": [0, 1],
            "y": [10, 20],
            "type": "scatter",
            "mode": "lines",
        }
    ]


def test_scatter_chart_uses_plotly_scatter_markers():
    frame = pd.DataFrame({"Time": [0, 1], "Speed": [10, 20]})

    payload = build_chart_payload(frame, "scatter", "Time", ["Speed"], None)

    assert payload["data"][0]["type"] == "scatter"
    assert payload["data"][0]["mode"] == "markers"
