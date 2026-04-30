from dash import Dash, Input, Output, State, ctx, dcc, html, no_update, dash_table
import pandas as pd
import plotly.express as px


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


df = load_log_data("log.csv")
all_channels = sorted(df["channel"].unique())
if not all_channels:
    raise ValueError("No channel data found in log.csv")

default_channels = all_channels[: min(8, len(all_channels))]
default_table_channel = all_channels[0]


def make_figure(frame: pd.DataFrame, y_col: str, title: str, y_label: str):
    fig = px.line(
        frame,
        x="timestamp",
        y=y_col,
        color="channel",
        title=title,
        labels={"timestamp": "Time (s)", y_col: y_label, "channel": "Channel"},
    )
    fig.update_layout(hovermode="x unified")
    return fig


def make_table_rows(frame: pd.DataFrame) -> list[dict]:
    table_df = frame[["channel", "temp", "timestamp"]].copy()
    table_df["temp"] = table_df["temp"].round(3)
    table_df["timestamp"] = table_df["timestamp"].round(3)
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

app = Dash()

app.layout = html.Div(
    [
        html.H3("Temperature Telemetry"),
        dcc.Dropdown(
            id="channel-select",
            options=[{"label": channel, "value": channel} for channel in all_channels],
            value=default_channels,
            multi=True,
            placeholder="Select one or more channels",
        ),
        html.Div(
            [
                html.Button("Select all", id="select-all-btn", n_clicks=0),
                html.Button("Clear", id="clear-btn", n_clicks=0),
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
        ),
        dash_table.DataTable(
            id="temp-table",
            data=make_table_rows(initial_table_df),
            columns=[
                {"name": "Channel", "id": "channel"},
                {"name": "Temp", "id": "temp", "type": "numeric", "format": {"specifier": ".3f"}},
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
                "border": "1px solid #d9d9d9",
                "borderRadius": "6px",
            },
            style_header={
                "fontWeight": "700",
                "backgroundColor": "#e9eef5",
                "color": "#1f2937",
                "fontSize": "15px",
                "border": "1px solid #d1d5db",
            },
            style_cell={
                "padding": "0.65rem",
                "textAlign": "left",
                "minWidth": "170px",
                "width": "170px",
                "maxWidth": "320px",
                "fontSize": "14px",
                "lineHeight": "1.4",
                "color": "#111827",
                "border": "1px solid #e5e7eb",
            },
            style_data_conditional=[
                {"if": {"row_index": "odd"}, "backgroundColor": "#f8fafc"},
                {"if": {"column_id": "temp"}, "textAlign": "right"},
                {"if": {"column_id": "timestamp"}, "textAlign": "right"},
            ],
            style_filter={"fontSize": "14px", "padding": "0.45rem", "color": "#111827"},
        ),
    ],
    style={"padding": "0.75rem 1rem 1.5rem", "backgroundColor": "#fcfcfd"},
)


@app.callback(
    Output("channel-select", "value"),
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

if __name__ == "__main__":
    app.run(debug=True)
