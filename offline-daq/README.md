# Offline DAQ Viewer

Offline desktop CSV reader for graphing any two columns from large CSV files.

## Run

```bash
uv sync --extra dev
uv run offline-daq-viewer
```

The app runs locally and does not upload data anywhere.

To open the first CSV matched from the DAQ data directory:

```bash
uv run offline-daq-viewer '../daq/data/*.csv'
```

## Features

- Select a local CSV file.
- Detect headers without loading the full file into memory.
- Pick one X column and one Y column.
- Load only the selected columns.
- Plot with downsampling for large datasets.
- Keep CSV loading off the UI thread.
