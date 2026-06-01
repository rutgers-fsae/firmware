import { useMemo, useState } from "react";
import type { SchemaColumn } from "../types/dataset";
import type { ChartRequest } from "../types/chart";

type Props = {
  columns: SchemaColumn[];
  onRun: (payload: ChartRequest, axisTitles: { xTitle: string; yTitle: string }) => void;
};

export function ChartBuilder({ columns, onRun }: Props) {
  const [chartType, setChartType] = useState<ChartRequest["chart_type"]>("line");
  const [xColumn, setXColumn] = useState("");
  const [yColumn, setYColumn] = useState("");

  const numericColumns = useMemo(() => columns.filter((col) => col.type === "numeric"), [columns]);

  function columnLabel(columnName: string) {
    const column = columns.find((item) => item.name === columnName);
    if (!column) return columnName;
    return column.display_name || column.name;
  }

  return (
    <section className="grid gap-3 rounded-2xl border border-border bg-panel p-4 backdrop-blur">
      <h3 className="text-lg font-semibold">Chart Builder</h3>
      <select
        value={chartType}
        onChange={(e) => setChartType(e.target.value as ChartRequest["chart_type"])}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      >
        <option value="line">Line</option>
        <option value="scatter">Scatter</option>
        <option value="bar">Bar</option>
        <option value="histogram">Histogram</option>
        <option value="box">Box</option>
      </select>
      <select
        value={xColumn}
        onChange={(e) => setXColumn(e.target.value)}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      >
        <option value="">X Axis</option>
        {columns.map((col) => (
          <option key={col.name} value={col.name}>{col.display_name || col.name}</option>
        ))}
      </select>
      <select
        value={yColumn}
        onChange={(e) => setYColumn(e.target.value)}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      >
        <option value="">Y Axis</option>
        {numericColumns.map((col) => (
          <option key={col.name} value={col.name}>{col.display_name || col.name}</option>
        ))}
      </select>
      <button
        disabled={!yColumn}
        onClick={() =>
          onRun(
            { chart_type: chartType, x_column: xColumn || undefined, y_columns: [yColumn], filters: [] },
            {
              xTitle: xColumn ? columnLabel(xColumn) : "Index",
              yTitle: columnLabel(yColumn),
            },
          )
        }
        className="rounded-lg bg-button px-3 py-2 text-sm font-medium text-button-text transition hover:bg-button-hover disabled:cursor-not-allowed disabled:bg-button-disabled"
      >
        Render
      </button>
    </section>
  );
}
