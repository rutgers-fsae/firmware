from dash import Dash, Input, Output, State, ctx, dcc, html, no_update
import dash_ag_grid as dag
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
default_channels = all_channels[: min(8, len(all_channels))]


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
    table_df = frame.copy()
    table_df["timestamp"] = table_df["timestamp"].round(3)
    table_df["temp"] = table_df["temp"].round(3)
    table_df["running_mean"] = table_df["running_mean"].round(3)
    return table_df.to_dict("records")

initial_df = df[df["channel"].isin(default_channels)]
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
        dag.AgGrid(
            id="temp-table",
            rowData=make_table_rows(initial_df),
            columnDefs=[{"field": col} for col in df.columns],
        ),
    ]
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
    Output("temp-table", "rowData"),
    Input("channel-select", "value"),
)
def update_channel_view(selected_channels):
    if not selected_channels:
        filtered_df = df.iloc[0:0]
    else:
        filtered_df = df[df["channel"].isin(selected_channels)]

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

    return raw_figure, running_figure, make_table_rows(filtered_df)

if __name__ == "__main__":
    app.run(debug=True)
