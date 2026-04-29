# Import packages
from dash import Dash, html, dcc
import dash_ag_grid as dag
import pandas as pd
import plotly.express as px

# Incorporate data
df = pd.read_csv("log.csv")
df["timestamp"] = pd.to_datetime(df["timestamp"])

bucket_cols = [col for col in df.columns if col.startswith("bucket ")]

# Reshape to one row per timestamp/bucket value
long_df = df.melt(
    id_vars=["timestamp"],
    value_vars=bucket_cols,
    var_name="bucket",
    value_name="temperature",
)

# Average temperature per bucket vs time
avg_temp_df = (
    long_df.groupby(["timestamp", "bucket"], as_index=False)["temperature"]
    .mean()
    .sort_values("timestamp")
)

fig = px.line(
    avg_temp_df,
    x="timestamp",
    y="temperature",
    color="bucket",
    markers=True,
    title="Average Temperature per Bucket vs Time",
    labels={"timestamp": "Time", "temperature": "Avg Temperature"},
)

fig.update_layout(hovermode="x unified")

# Initialize the app
app = Dash()

# App layout
app.layout = html.Div(
    [
        html.H3("Average Temperature per Bucket vs Time"),
        dcc.Graph(figure=fig),
        dag.AgGrid(
            rowData=df.to_dict("records"),
            columnDefs=[{"field": col} for col in df.columns],
        ),
    ]
)

# Run the app
if __name__ == "__main__":
    app.run(debug=True)
