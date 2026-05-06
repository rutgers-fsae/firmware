from dash import Dash, Input, Output, State, ctx, dcc, html, no_update, dash_table
import glob
import os
import pandas as pd
import plotly.express as px


def find_latest_csv(pattern: str) -> str | None:
    matches = glob.glob(pattern)
    if not matches:
        return None
    return max(matches, key=os.path.getmtime)


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


temp_log_path = find_latest_csv("log-*.csv")
if temp_log_path is None:
    raise ValueError("No temperature log file found matching log-*.csv")

df = load_log_data(temp_log_path)
all_channels = sorted(df["channel"].unique())
if not all_channels:
    raise ValueError("No channel data found in log.csv")

default_channels = all_channels[: min(8, len(all_channels))]
default_table_channel = all_channels[0]

daq_log_path = find_latest_csv("daq-log-*.csv")
if daq_log_path is None:
    daq_df = pd.DataFrame(columns=["timestamp", "voltage"])
    daq_status_text = "No DAQ log file found matching daq-log-*.csv"
else:
    daq_df = load_daq_data(daq_log_path)
    daq_status_text = f"Using DAQ log: {os.path.basename(daq_log_path)}"


def reload_latest_data():
    global df, all_channels, default_channels, default_table_channel, daq_df, daq_status_text

    temp_log = find_latest_csv("log-*.csv")
    if temp_log is None:
        raise ValueError("No temperature log file found matching log-*.csv")

    next_df = load_log_data(temp_log)
    next_channels = sorted(next_df["channel"].unique())
    if not next_channels:
        raise ValueError("No channel data found in latest temperature log")

    df = next_df
    all_channels = next_channels
    default_channels = all_channels[: min(8, len(all_channels))]
    default_table_channel = all_channels[0]

    latest_daq_log = find_latest_csv("daq-log-*.csv")
    if latest_daq_log is None:
        daq_df = pd.DataFrame(columns=["timestamp", "voltage"])
        daq_status_text = "No DAQ log file found matching daq-log-*.csv"
    else:
        daq_df = load_daq_data(latest_daq_log)
        daq_status_text = f"Using DAQ log: {os.path.basename(latest_daq_log)}"


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
    Output("daq-status", "children"),
    Output("daq-voltage-graph", "figure"),
    Output("daq-table", "data"),
    Input("refresh-logs-btn", "n_clicks"),
    prevent_initial_call=True,
)
def refresh_latest_logs(_n_clicks):
    reload_latest_data()

    channel_options = [{"label": channel, "value": channel} for channel in all_channels]
    table_options = [{"label": channel, "value": channel} for channel in all_channels]
    daq_figure = make_daq_figure(daq_df)

    return (
        channel_options,
        default_channels,
        table_options,
        default_table_channel,
        daq_status_text,
        daq_figure,
        make_daq_table_rows(daq_df),
    )

if __name__ == "__main__":
    app.run(debug=True)
