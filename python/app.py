from dash import Dash, Input, Output, State, ctx, dcc, html, no_update, dash_table
from datetime import datetime
import glob
import os
import pandas as pd
import plotly.express as px


def find_latest_csv(pattern: str) -> str | None:
    matches = glob.glob(pattern, recursive=True)
    if not matches:
        return None
    return max(matches, key=os.path.getmtime)


def list_csv_files(pattern: str) -> list[str]:
    matches = glob.glob(pattern, recursive=True)
    return sorted(matches, key=os.path.getmtime)


def list_csv_files_multi(patterns: list[str]) -> list[str]:
    merged: set[str] = set()
    for pattern in patterns:
        merged.update(list_csv_files(pattern))
    return sorted(merged, key=os.path.getmtime)


def format_csv_label(path: str) -> str:
    basename = os.path.basename(path)
    run_dir = os.path.basename(os.path.dirname(path))
    if run_dir.startswith("run-"):
        run_ts = run_dir[4:]
        try:
            dt = datetime.strptime(run_ts, "%Y-%m-%d_%H-%M-%S")
            return f"{dt.strftime('%Y-%m-%d %H:%M:%S')} - {basename}"
        except ValueError:
            pass

    name_without_ext, _ext = os.path.splitext(basename)
    ts_str = name_without_ext.rsplit("-", 1)[-1]
    try:
        dt = datetime.fromtimestamp(int(ts_str))
        return f"{dt.strftime('%Y-%m-%d %H:%M:%S')} - {basename}"
    except (TypeError, ValueError, OSError):
        return basename


def make_file_options(paths: list[str]) -> list[dict[str, str]]:
    return [{"label": format_csv_label(path), "value": path} for path in paths]


TEMP_LOG_PATTERNS = ["logs/**/temperature.csv", "logs/log-*.csv"]
DAQ_LOG_PATTERNS = ["logs/**/daq.csv", "logs/daq-log-*.csv"]
FAULT_LOG_PATTERNS = ["logs/**/fault.csv", "logs/fault-log-*.csv"]
INV_LOG_PATTERNS = ["logs/**/inverter.csv", "logs/inv-log-*.csv"]


def load_log_data(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    required_cols = {"timestamp", "channel", "temp"}
    missing_cols = required_cols - set(df.columns)
    if missing_cols:
        raise ValueError(f"Missing required columns in {path}: {sorted(missing_cols)}")

    df["timestamp"] = pd.to_numeric(df["timestamp"], errors="coerce")
    df["temp"] = pd.to_numeric(df["temp"], errors="coerce")
    df = df.dropna(subset=["timestamp", "channel", "temp"]).copy()
    df = df.sort_values(["channel", "timestamp"]).reset_index(drop=True)

    df["running_mean"] = (
        df.groupby("channel")["temp"]
        .transform(lambda series: series.expanding().mean())
        .astype(float)
    )

    return df


def load_daq_data(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    required_cols = {"timestamp", "voltage"}
    missing_cols = required_cols - set(df.columns)
    if missing_cols:
        raise ValueError(f"Missing required columns in {path}: {sorted(missing_cols)}")

    df["timestamp"] = pd.to_numeric(df["timestamp"], errors="coerce")
    df["voltage"] = pd.to_numeric(df["voltage"], errors="coerce")
    df = df.dropna(subset=["timestamp", "voltage"]).copy()
    df = df.sort_values(["timestamp"]).reset_index(drop=True)
    return df


def load_fault_data(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    required_cols = {"timestamp", "high", "low", "faults"}
    missing_cols = required_cols - set(df.columns)
    if missing_cols:
        raise ValueError(f"Missing required columns in {path}: {sorted(missing_cols)}")

    df["timestamp"] = pd.to_numeric(df["timestamp"], errors="coerce")
    df["faults"] = pd.to_numeric(df["faults"], errors="coerce")
    df["high"] = pd.to_numeric(df["high"], errors="coerce")
    df["low"] = pd.to_numeric(df["low"], errors="coerce")
    df = df.dropna(subset=["timestamp", "faults"]).copy()
    df = df.sort_values(["timestamp"]).reset_index(drop=True)
    return df


def load_inv_data(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    required_cols = {"timestamp", "inv_byte0"}
    missing_cols = required_cols - set(df.columns)
    if missing_cols:
        raise ValueError(f"Missing required columns in {path}: {sorted(missing_cols)}")

    df["timestamp"] = pd.to_numeric(df["timestamp"], errors="coerce")
    df["inv_byte0"] = pd.to_numeric(df["inv_byte0"], errors="coerce")
    if "dlc" in df.columns:
        df["dlc"] = pd.to_numeric(df["dlc"], errors="coerce")
    df = df.dropna(subset=["timestamp"]).copy()
    df = df.sort_values(["timestamp"]).reset_index(drop=True)
    return df


temp_log_files = list_csv_files_multi(TEMP_LOG_PATTERNS)
if not temp_log_files:
    raise ValueError("No temperature log file found matching log-*.csv")

temp_log_path = temp_log_files[-1]
temp_log_options = make_file_options(temp_log_files)
temp_status_text = f"Using temperature log: {os.path.basename(temp_log_path)}"

df = load_log_data(temp_log_path)
all_channels = sorted(df["channel"].unique())
if not all_channels:
    raise ValueError("No channel data found in log.csv")

default_channels = all_channels[: min(8, len(all_channels))]
default_table_channel = all_channels[0]

daq_log_files = list_csv_files_multi(DAQ_LOG_PATTERNS)
daq_log_options = make_file_options(daq_log_files)
daq_log_path = daq_log_files[-1] if daq_log_files else None
if daq_log_path is None:
    daq_df = pd.DataFrame(columns=["timestamp", "voltage"])
    daq_status_text = "No DAQ log file found matching daq-log-*.csv"
else:
    daq_df = load_daq_data(daq_log_path)
    daq_status_text = f"Using DAQ log: {os.path.basename(daq_log_path)}"

fault_log_files = list_csv_files_multi(FAULT_LOG_PATTERNS)
fault_log_options = make_file_options(fault_log_files)
fault_log_path = fault_log_files[-1] if fault_log_files else None
if fault_log_path is None:
    fault_df = pd.DataFrame(columns=["timestamp", "high", "low", "faults"])
    fault_status_text = "No fault log file found matching fault-log-*.csv"
else:
    fault_df = load_fault_data(fault_log_path)
    fault_status_text = f"Using fault log: {os.path.basename(fault_log_path)}"

inv_log_files = list_csv_files_multi(INV_LOG_PATTERNS)
inv_log_options = make_file_options(inv_log_files)
inv_log_path = inv_log_files[-1] if inv_log_files else None
if inv_log_path is None:
    inv_df = pd.DataFrame(columns=["timestamp", "dlc", "inv_byte0"])
    inv_status_text = "No inverter log file found matching inv-log-*.csv"
else:
    inv_df = load_inv_data(inv_log_path)
    inv_status_text = f"Using inverter log: {os.path.basename(inv_log_path)}"


def reload_temp_data(path: str):
    global df, all_channels, default_channels, default_table_channel, temp_log_path, temp_status_text

    next_df = load_log_data(path)
    next_channels = sorted(next_df["channel"].unique())
    if not next_channels:
        raise ValueError("No channel data found in selected temperature log")

    df = next_df
    temp_log_path = path
    temp_status_text = f"Using temperature log: {os.path.basename(path)}"
    all_channels = next_channels
    default_channels = all_channels[: min(8, len(all_channels))]
    default_table_channel = all_channels[0]


def reload_daq_data(path: str | None):
    global daq_df, daq_status_text, daq_log_path

    daq_log_path = path
    if path is None:
        daq_df = pd.DataFrame(columns=["timestamp", "voltage"])
        daq_status_text = "No DAQ log file found matching daq-log-*.csv"
    else:
        daq_df = load_daq_data(path)
        daq_status_text = f"Using DAQ log: {os.path.basename(path)}"


def reload_fault_data(path: str | None):
    global fault_df, fault_status_text, fault_log_path

    fault_log_path = path
    if path is None:
        fault_df = pd.DataFrame(columns=["timestamp", "high", "low", "faults"])
        fault_status_text = "No fault log file found matching fault-log-*.csv"
    else:
        fault_df = load_fault_data(path)
        fault_status_text = f"Using fault log: {os.path.basename(path)}"


def reload_inv_data(path: str | None):
    global inv_df, inv_status_text, inv_log_path

    inv_log_path = path
    if path is None:
        inv_df = pd.DataFrame(columns=["timestamp", "dlc", "inv_byte0"])
        inv_status_text = "No inverter log file found matching inv-log-*.csv"
    else:
        inv_df = load_inv_data(path)
        inv_status_text = f"Using inverter log: {os.path.basename(path)}"


def make_figure(frame: pd.DataFrame, y_col: str, title: str, y_label: str):
    fig = px.line(
        frame,
        x="timestamp",
        y=y_col,
        color="channel",
        title=title,
        labels={"timestamp": "Time (s)", y_col: y_label, "channel": "Channel"},
        markers=True
    )
    fig.update_layout(
        template="plotly_dark",
        paper_bgcolor="#0f172a",
        plot_bgcolor="#111827",
        font={"color": "#e5e7eb"},
        legend={"bgcolor": "rgba(17,24,39,0.7)", "bordercolor": "#334155", "borderwidth": 1},
    )
    return fig


def make_table_rows(frame: pd.DataFrame) -> list[dict]:
    table_df = frame[["channel", "temp", "timestamp"]].copy()
    table_df["temp"] = table_df["temp"].round(3)
    table_df["timestamp"] = table_df["timestamp"].round(3)
    return table_df.to_dict("records")


def make_daq_figure(frame: pd.DataFrame):
    fig = px.line(
        frame,
        x="timestamp",
        y="voltage",
        title="DAQ Voltage vs Time",
        labels={"timestamp": "Time (s)", "voltage": "Voltage (V)"},
        markers=True,
    )
    fig.update_layout(
        template="plotly_dark",
        paper_bgcolor="#0f172a",
        plot_bgcolor="#111827",
        font={"color": "#e5e7eb"},
    )
    return fig


def make_daq_table_rows(frame: pd.DataFrame) -> list[dict]:
    table_df = frame[["timestamp", "voltage"]].copy()
    table_df["timestamp"] = table_df["timestamp"].round(3)
    table_df["voltage"] = table_df["voltage"].round(3)
    return table_df.to_dict("records")


def make_fault_figure(frame: pd.DataFrame):
    fig = px.line(
        frame,
        x="timestamp",
        y=["faults", "high", "low"],
        title="Fault Metrics vs Time",
        labels={
            "timestamp": "Time (s)",
            "value": "Value",
            "variable": "Signal",
            "faults": "Faults",
            "high": "High",
            "low": "Low",
        },
        markers=True,
    )
    fig.update_layout(
        template="plotly_dark",
        paper_bgcolor="#0f172a",
        plot_bgcolor="#111827",
        font={"color": "#e5e7eb"},
    )
    fig.for_each_trace(
        lambda trace: trace.update(
            line={
                "color": {
                    "faults": "#ef4444",
                    "high": "#f59e0b",
                    "low": "#38bdf8",
                }.get(trace.name, trace.line.color)
            }
        )
    )
    return fig


def make_fault_table_rows(frame: pd.DataFrame) -> list[dict]:
    table_df = frame[["timestamp", "high", "low", "faults"]].copy()
    table_df["timestamp"] = table_df["timestamp"].round(3)
    table_df["faults"] = table_df["faults"].astype(int)
    table_df["high"] = table_df["high"].astype(int)
    table_df["low"] = table_df["low"].astype(int)
    return table_df.to_dict("records")


def make_inv_figure(frame: pd.DataFrame):
    fig = px.line(
        frame,
        x="timestamp",
        y="inv_byte0",
        title="Inverter Byte 0 vs Time",
        labels={"timestamp": "Time (s)", "inv_byte0": "Byte 0 (raw)"},
        markers=True,
    )
    fig.update_layout(
        template="plotly_dark",
        paper_bgcolor="#0f172a",
        plot_bgcolor="#111827",
        font={"color": "#e5e7eb"},
    )
    return fig


def make_inv_table_rows(frame: pd.DataFrame) -> list[dict]:
    table_df = frame[["timestamp", "dlc", "inv_byte0"]].copy()
    table_df["timestamp"] = table_df["timestamp"].round(3)
    table_df["dlc"] = pd.to_numeric(table_df["dlc"], errors="coerce").round(0).astype("Int64")
    table_df["inv_byte0"] = pd.to_numeric(table_df["inv_byte0"], errors="coerce").round(0).astype("Int64")
    return table_df.to_dict("records")

initial_df = df[df["channel"].isin(default_channels)]
initial_table_df = df[df["channel"] == default_table_channel]
raw_temp_fig = make_figure(
    initial_df,
    y_col="temp",
    title="Raw Temperature vs Time",
    y_label="Temperature",
)
running_mean_fig = make_figure(
    initial_df,
    y_col="running_mean",
    title="Running Mean Temperature vs Time",
    y_label="Running Mean Temperature",
)
daq_fig = make_daq_figure(daq_df)
fault_fig = make_fault_figure(fault_df)
inv_fig = make_inv_figure(inv_df)

app = Dash()

app.layout = html.Div(
    [
        html.H3("Temperature Telemetry"),
        html.Button(
            "Refresh latest logs",
            id="refresh-logs-btn",
            n_clicks=0,
            style={
                "backgroundColor": "#047857",
                "color": "#f8fafc",
                "border": "1px solid #10b981",
                "padding": "0.45rem 0.8rem",
                "borderRadius": "6px",
                "marginBottom": "0.6rem",
            },
        ),
        html.H4("Temperature CSV", style={"margin": "0.2rem 0"}),
        dcc.Dropdown(
            id="temp-log-select",
            options=temp_log_options,
            value=temp_log_path,
            multi=False,
            clearable=False,
            searchable=True,
            placeholder="Select temperature CSV",
            style={"maxWidth": "720px", "marginBottom": "0.5rem"},
            className="dark-dropdown",
        ),
        html.Div(temp_status_text, id="temp-status", style={"marginBottom": "0.4rem", "color": "#cbd5e1"}),
        dcc.Dropdown(
            id="channel-select",
            options=[{"label": channel, "value": channel} for channel in all_channels],
            value=default_channels,
            multi=True,
            placeholder="Select one or more channels",
            className="dark-dropdown",
        ),
        html.Div(
            [
                html.Button(
                    "Select all",
                    id="select-all-btn",
                    n_clicks=0,
                    style={
                        "backgroundColor": "#1d4ed8",
                        "color": "#f8fafc",
                        "border": "1px solid #3b82f6",
                        "padding": "0.45rem 0.8rem",
                        "borderRadius": "6px",
                    },
                ),
                html.Button(
                    "Clear",
                    id="clear-btn",
                    n_clicks=0,
                    style={
                        "backgroundColor": "#1f2937",
                        "color": "#f8fafc",
                        "border": "1px solid #334155",
                        "padding": "0.45rem 0.8rem",
                        "borderRadius": "6px",
                    },
                ),
            ],
            style={"display": "flex", "gap": "0.5rem", "margin": "0.75rem 0"},
        ),
        dcc.Graph(id="raw-temp-graph", figure=raw_temp_fig),
        dcc.Graph(id="running-mean-graph", figure=running_mean_fig),
        html.H4("Temperature Table by Channel", style={"margin": "0.5rem 0 0"}),
        dcc.Dropdown(
            id="table-channel-select",
            options=[{"label": channel, "value": channel} for channel in all_channels],
            value=default_table_channel,
            multi=False,
            clearable=False,
            searchable=True,
            placeholder="Select one channel for table",
            style={"maxWidth": "360px", "marginBottom": "0.5rem"},
            className="dark-dropdown",
        ),
        dash_table.DataTable(
            id="temp-table",
            data=make_table_rows(initial_table_df),
            columns=[
                {"name": "Channel", "id": "channel"},
                {"name": "Temp (c)", "id": "temp", "type": "numeric", "format": {"specifier": ".3f"}},
                {
                    "name": "Time Recorded (since start)",
                    "id": "timestamp",
                    "type": "numeric",
                    "format": {"specifier": ".3f"},
                },
            ],
            sort_action="native",
            filter_action="native",
            page_action="native",
            page_size=15,
            fixed_rows={"headers": True},
            style_table={
                "height": "420px",
                "overflowY": "auto",
                "overflowX": "auto",
                "marginTop": "0.5rem",
                "border": "1px solid #334155",
                "borderRadius": "6px",
                "backgroundColor": "#0b1220",
            },
            style_header={
                "fontWeight": "700",
                "backgroundColor": "#1e293b",
                "color": "#f8fafc",
                "fontSize": "15px",
                "border": "1px solid #334155",
            },
            style_cell={
                "padding": "0.65rem",
                "textAlign": "left",
                "minWidth": "170px",
                "width": "170px",
                "maxWidth": "320px",
                "fontSize": "14px",
                "lineHeight": "1.4",
                "color": "#e5e7eb",
                "border": "1px solid #1e293b",
                "backgroundColor": "#111827",
            },
            style_data_conditional=[
                {"if": {"row_index": "odd"}, "backgroundColor": "#0f172a"},
                {"if": {"column_id": "temp"}, "textAlign": "right"},
                {"if": {"column_id": "timestamp"}, "textAlign": "right"},
            ],
            style_filter={
                "fontSize": "14px",
                "padding": "0.45rem",
                "color": "#e5e7eb",
                "backgroundColor": "#0f172a",
                "border": "1px solid #334155",
            },
        ),
        html.H3("DAQ Voltage Telemetry", style={"margin": "1.2rem 0 0.2rem"}),
        dcc.Dropdown(
            id="daq-log-select",
            options=daq_log_options,
            value=daq_log_path,
            multi=False,
            clearable=False,
            searchable=True,
            placeholder="Select DAQ CSV",
            style={"maxWidth": "720px", "marginBottom": "0.5rem"},
            className="dark-dropdown",
        ),
        html.Div(daq_status_text, id="daq-status", style={"marginBottom": "0.4rem", "color": "#cbd5e1"}),
        dcc.Graph(id="daq-voltage-graph", figure=daq_fig),
        dash_table.DataTable(
            id="daq-table",
            data=make_daq_table_rows(daq_df),
            columns=[
                {
                    "name": "Time Recorded (since start)",
                    "id": "timestamp",
                    "type": "numeric",
                    "format": {"specifier": ".3f"},
                },
                {
                    "name": "Voltage (V)",
                    "id": "voltage",
                    "type": "numeric",
                    "format": {"specifier": ".3f"},
                },
            ],
            sort_action="native",
            filter_action="native",
            fixed_rows={"headers": True},
            style_table={
                "height": "720px",
                "overflowY": "auto",
                "overflowX": "auto",
                "marginTop": "0.5rem",
                "border": "1px solid #334155",
                "borderRadius": "6px",
                "backgroundColor": "#0b1220",
            },
            style_header={
                "fontWeight": "700",
                "backgroundColor": "#1e293b",
                "color": "#f8fafc",
                "fontSize": "15px",
                "border": "1px solid #334155",
            },
            style_cell={
                "padding": "0.65rem",
                "textAlign": "left",
                "minWidth": "170px",
                "width": "170px",
                "maxWidth": "320px",
                "fontSize": "14px",
                "lineHeight": "1.4",
                "color": "#e5e7eb",
                "border": "1px solid #1e293b",
                "backgroundColor": "#111827",
            },
            style_data_conditional=[
                {"if": {"row_index": "odd"}, "backgroundColor": "#0f172a"},
                {"if": {"column_id": "timestamp"}, "textAlign": "right"},
                {"if": {"column_id": "voltage"}, "textAlign": "right"},
            ],
            style_filter={
                "fontSize": "14px",
                "padding": "0.45rem",
                "color": "#e5e7eb",
                "backgroundColor": "#0f172a",
                "border": "1px solid #334155",
            },
        ),
        html.H3("Fault Telemetry", style={"margin": "1.2rem 0 0.2rem"}),
        dcc.Dropdown(
            id="fault-log-select",
            options=fault_log_options,
            value=fault_log_path,
            multi=False,
            clearable=False,
            searchable=True,
            placeholder="Select fault CSV",
            style={"maxWidth": "720px", "marginBottom": "0.5rem"},
            className="dark-dropdown",
        ),
        html.Div(fault_status_text, id="fault-status", style={"marginBottom": "0.4rem", "color": "#cbd5e1"}),
        dcc.Graph(id="fault-graph", figure=fault_fig),
        dash_table.DataTable(
            id="fault-table",
            data=make_fault_table_rows(fault_df),
            columns=[
                {
                    "name": "Time Recorded (since start)",
                    "id": "timestamp",
                    "type": "numeric",
                    "format": {"specifier": ".3f"},
                },
                {
                    "name": "Faults",
                    "id": "faults",
                    "type": "numeric",
                    "format": {"specifier": "d"},
                },
            ],
            sort_action="native",
            filter_action="native",
            fixed_rows={"headers": True},
            style_table={
                "height": "420px",
                "overflowY": "auto",
                "overflowX": "auto",
                "marginTop": "0.5rem",
                "border": "1px solid #334155",
                "borderRadius": "6px",
                "backgroundColor": "#0b1220",
            },
            style_header={
                "fontWeight": "700",
                "backgroundColor": "#1e293b",
                "color": "#f8fafc",
                "fontSize": "15px",
                "border": "1px solid #334155",
            },
            style_cell={
                "padding": "0.65rem",
                "textAlign": "left",
                "minWidth": "170px",
                "width": "170px",
                "maxWidth": "320px",
                "fontSize": "14px",
                "lineHeight": "1.4",
                "color": "#e5e7eb",
                "border": "1px solid #1e293b",
                "backgroundColor": "#111827",
            },
            style_data_conditional=[
                {"if": {"row_index": "odd"}, "backgroundColor": "#0f172a"},
                {"if": {"column_id": "timestamp"}, "textAlign": "right"},
                {"if": {"column_id": "faults"}, "textAlign": "right"},
            ],
            style_filter={
                "fontSize": "14px",
                "padding": "0.45rem",
                "color": "#e5e7eb",
                "backgroundColor": "#0f172a",
                "border": "1px solid #334155",
            },
        ),
        html.H3("Inverter Telemetry", style={"margin": "1.2rem 0 0.2rem"}),
        dcc.Dropdown(
            id="inv-log-select",
            options=inv_log_options,
            value=inv_log_path,
            multi=False,
            clearable=False,
            searchable=True,
            placeholder="Select inverter CSV",
            style={"maxWidth": "720px", "marginBottom": "0.5rem"},
            className="dark-dropdown",
        ),
        html.Div(inv_status_text, id="inv-status", style={"marginBottom": "0.4rem", "color": "#cbd5e1"}),
        dcc.Graph(id="inv-graph", figure=inv_fig),
        dash_table.DataTable(
            id="inv-table",
            data=make_inv_table_rows(inv_df),
            columns=[
                {
                    "name": "Time Recorded (since start)",
                    "id": "timestamp",
                    "type": "numeric",
                    "format": {"specifier": ".3f"},
                },
                {
                    "name": "DLC",
                    "id": "dlc",
                    "type": "numeric",
                    "format": {"specifier": "d"},
                },
                {
                    "name": "Byte 0 (raw)",
                    "id": "inv_byte0",
                    "type": "numeric",
                    "format": {"specifier": "d"},
                },
            ],
            sort_action="native",
            filter_action="native",
            fixed_rows={"headers": True},
            style_table={
                "height": "420px",
                "overflowY": "auto",
                "overflowX": "auto",
                "marginTop": "0.5rem",
                "border": "1px solid #334155",
                "borderRadius": "6px",
                "backgroundColor": "#0b1220",
            },
            style_header={
                "fontWeight": "700",
                "backgroundColor": "#1e293b",
                "color": "#f8fafc",
                "fontSize": "15px",
                "border": "1px solid #334155",
            },
            style_cell={
                "padding": "0.65rem",
                "textAlign": "left",
                "minWidth": "170px",
                "width": "170px",
                "maxWidth": "320px",
                "fontSize": "14px",
                "lineHeight": "1.4",
                "color": "#e5e7eb",
                "border": "1px solid #1e293b",
                "backgroundColor": "#111827",
            },
            style_data_conditional=[
                {"if": {"row_index": "odd"}, "backgroundColor": "#0f172a"},
                {"if": {"column_id": "timestamp"}, "textAlign": "right"},
                {"if": {"column_id": "dlc"}, "textAlign": "right"},
                {"if": {"column_id": "inv_byte0"}, "textAlign": "right"},
            ],
            style_filter={
                "fontSize": "14px",
                "padding": "0.45rem",
                "color": "#e5e7eb",
                "backgroundColor": "#0f172a",
                "border": "1px solid #334155",
            },
        ),
    ],
    style={
        "padding": "0.75rem 1rem 1.5rem",
        "background": "linear-gradient(180deg, #020617 0%, #0f172a 100%)",
        "color": "#e5e7eb",
        "minHeight": "100vh",
    },
)


@app.callback(
    Output("channel-select", "value", allow_duplicate=True),
    Input("select-all-btn", "n_clicks"),
    Input("clear-btn", "n_clicks"),
    State("channel-select", "value"),
    prevent_initial_call=True,
)
def update_channel_selection(_select_all_clicks, _clear_clicks, selected_channels):
    triggered_id = ctx.triggered_id
    if triggered_id == "select-all-btn":
        return all_channels
    if triggered_id == "clear-btn":
        return []
    return selected_channels if selected_channels is not None else no_update


@app.callback(
    Output("raw-temp-graph", "figure"),
    Output("running-mean-graph", "figure"),
    Output("temp-table", "data"),
    Input("channel-select", "value"),
    Input("table-channel-select", "value"),
)
def update_channel_view(selected_channels, selected_table_channel):
    if not selected_channels:
        filtered_df = df.iloc[0:0]
    else:
        filtered_df = df[df["channel"].isin(selected_channels)]

    table_channel = selected_table_channel or default_table_channel
    table_df = df[df["channel"] == table_channel]

    raw_figure = make_figure(
        filtered_df,
        y_col="temp",
        title="Raw Temperature vs Time",
        y_label="Temperature",
    )
    running_figure = make_figure(
        filtered_df,
        y_col="running_mean",
        title="Running Mean Temperature vs Time",
        y_label="Running Mean Temperature",
    )

    return raw_figure, running_figure, make_table_rows(table_df)


@app.callback(
    Output("channel-select", "options"),
    Output("channel-select", "value"),
    Output("table-channel-select", "options"),
    Output("table-channel-select", "value"),
    Output("temp-status", "children"),
    Input("temp-log-select", "value"),
)
def switch_temperature_log(selected_temp_log):
    if not selected_temp_log or not os.path.exists(selected_temp_log):
        return no_update, no_update, no_update, no_update, no_update

    reload_temp_data(selected_temp_log)
    channel_options = [{"label": channel, "value": channel} for channel in all_channels]
    table_options = [{"label": channel, "value": channel} for channel in all_channels]
    return channel_options, default_channels, table_options, default_table_channel, temp_status_text


@app.callback(
    Output("daq-status", "children"),
    Output("daq-voltage-graph", "figure"),
    Output("daq-table", "data"),
    Input("daq-log-select", "value"),
)
def switch_daq_log(selected_daq_log):
    if selected_daq_log is not None and not os.path.exists(selected_daq_log):
        return "Selected DAQ log file no longer exists", make_daq_figure(pd.DataFrame(columns=["timestamp", "voltage"])), []

    reload_daq_data(selected_daq_log)
    return daq_status_text, make_daq_figure(daq_df), make_daq_table_rows(daq_df)


@app.callback(
    Output("fault-status", "children"),
    Output("fault-graph", "figure"),
    Output("fault-table", "data"),
    Input("fault-log-select", "value"),
)
def switch_fault_log(selected_fault_log):
    if selected_fault_log is not None and not os.path.exists(selected_fault_log):
        return "Selected fault log file no longer exists", make_fault_figure(pd.DataFrame(columns=["timestamp", "faults"])), []

    reload_fault_data(selected_fault_log)
    return fault_status_text, make_fault_figure(fault_df), make_fault_table_rows(fault_df)


@app.callback(
    Output("inv-status", "children"),
    Output("inv-graph", "figure"),
    Output("inv-table", "data"),
    Input("inv-log-select", "value"),
)
def switch_inv_log(selected_inv_log):
    if selected_inv_log is not None and not os.path.exists(selected_inv_log):
        return (
            "Selected inverter log file no longer exists",
            make_inv_figure(pd.DataFrame(columns=["timestamp", "dlc", "inv_byte0"])),
            [],
        )

    reload_inv_data(selected_inv_log)
    return inv_status_text, make_inv_figure(inv_df), make_inv_table_rows(inv_df)


@app.callback(
    Output("temp-log-select", "options"),
    Output("temp-log-select", "value"),
    Output("daq-log-select", "options"),
    Output("daq-log-select", "value"),
    Output("fault-log-select", "options"),
    Output("fault-log-select", "value"),
    Output("inv-log-select", "options"),
    Output("inv-log-select", "value"),
    Output("channel-select", "options", allow_duplicate=True),
    Output("channel-select", "value", allow_duplicate=True),
    Output("table-channel-select", "options", allow_duplicate=True),
    Output("table-channel-select", "value", allow_duplicate=True),
    Output("temp-status", "children", allow_duplicate=True),
    Output("daq-status", "children", allow_duplicate=True),
    Output("daq-voltage-graph", "figure", allow_duplicate=True),
    Output("daq-table", "data", allow_duplicate=True),
    Output("fault-status", "children", allow_duplicate=True),
    Output("fault-graph", "figure", allow_duplicate=True),
    Output("fault-table", "data", allow_duplicate=True),
    Output("inv-status", "children", allow_duplicate=True),
    Output("inv-graph", "figure", allow_duplicate=True),
    Output("inv-table", "data", allow_duplicate=True),
    Input("refresh-logs-btn", "n_clicks"),
    prevent_initial_call=True,
)
def refresh_latest_logs(_n_clicks):
    latest_temp_files = list_csv_files_multi(TEMP_LOG_PATTERNS)
    latest_daq_files = list_csv_files_multi(DAQ_LOG_PATTERNS)
    latest_fault_files = list_csv_files_multi(FAULT_LOG_PATTERNS)
    latest_inv_files = list_csv_files_multi(INV_LOG_PATTERNS)
    if not latest_temp_files:
        return (
            [],
            None,
            make_file_options(latest_daq_files),
            latest_daq_files[-1] if latest_daq_files else None,
            make_file_options(latest_fault_files),
            latest_fault_files[-1] if latest_fault_files else None,
            make_file_options(latest_inv_files),
            latest_inv_files[-1] if latest_inv_files else None,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
            no_update,
        )

    latest_temp_log = latest_temp_files[-1]
    reload_temp_data(latest_temp_log)

    latest_daq_log = latest_daq_files[-1] if latest_daq_files else None
    reload_daq_data(latest_daq_log)

    latest_fault_log = latest_fault_files[-1] if latest_fault_files else None
    reload_fault_data(latest_fault_log)

    latest_inv_log = latest_inv_files[-1] if latest_inv_files else None
    reload_inv_data(latest_inv_log)

    channel_options = [{"label": channel, "value": channel} for channel in all_channels]
    table_options = [{"label": channel, "value": channel} for channel in all_channels]
    daq_figure = make_daq_figure(daq_df)

    return (
        make_file_options(latest_temp_files),
        latest_temp_log,
        make_file_options(latest_daq_files),
        latest_daq_log,
        make_file_options(latest_fault_files),
        latest_fault_log,
        make_file_options(latest_inv_files),
        latest_inv_log,
        channel_options,
        default_channels,
        table_options,
        default_table_channel,
        temp_status_text,
        daq_status_text,
        daq_figure,
        make_daq_table_rows(daq_df),
        fault_status_text,
        make_fault_figure(fault_df),
        make_fault_table_rows(fault_df),
        inv_status_text,
        make_inv_figure(inv_df),
        make_inv_table_rows(inv_df),
    )

if __name__ == "__main__":
    app.run(debug=True)
