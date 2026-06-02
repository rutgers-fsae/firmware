import { useMemo, useState } from "react";
import type { SchemaColumn } from "../types/dataset";
import type { ChartRequest } from "../types/chart";

type Props = {
  columns: SchemaColumn[];
  onRun: (
    payload: ChartRequest,
    axisTitles: { xTitle: string; yTitle: string; traceLabels: Record<string, string> },
  ) => void;
};

export function ChartBuilder({ columns, onRun }: Props) {
  const [chartType, setChartType] = useState<ChartRequest["chart_type"]>("line");
  const [xColumn, setXColumn] = useState("");
  const [yColumns, setYColumns] = useState<string[]>([]);

  const numericColumns = useMemo(() => columns.filter((col) => col.type === "numeric"), [columns]);

  function columnLabel(columnName: string) {
    const column = columns.find((item) => item.name === columnName);
    if (!column) return columnName;
    return column.display_name || column.name;
  }

  function columnUnit(columnName: string) {
    return columns.find((item) => item.name === columnName)?.unit || null;
  }

  function toggleYColumn(columnName: string) {
    setYColumns((prev) =>
      prev.includes(columnName) ? prev.filter((item) => item !== columnName) : [...prev, columnName],
    );
  }

  function yAxisTitle() {
    if (yColumns.length === 1) {
      return columnLabel(yColumns[0]);
    }

    const units = Array.from(new Set(yColumns.map(columnUnit).filter(Boolean)));
    return units.length === 1 ? `Values (${units[0]})` : "Values";
  }

  const mixedUnits = Array.from(new Set(yColumns.map(columnUnit).filter(Boolean))).length > 1;

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
      <div className="grid gap-2 rounded-lg border border-input-border bg-input p-3">
        <p className="text-sm font-medium text-text">Y Axis Series</p>
        {numericColumns.map((col) => (
          <label key={col.name} className="flex items-center gap-2 text-sm text-text">
            <input
              type="checkbox"
              checked={yColumns.includes(col.name)}
              onChange={() => toggleYColumn(col.name)}
              className="h-4 w-4 accent-button"
            />
            <span>{col.display_name || col.name}</span>
          </label>
        ))}
      </div>
      {yColumns.length > 0 && (
        <div className="flex flex-wrap gap-2">
          {yColumns.map((column) => (
            <button
              key={column}
              type="button"
              onClick={() => toggleYColumn(column)}
              className="rounded-full border border-input-border bg-input px-2 py-1 text-xs text-text transition hover:bg-button hover:text-button-text"
              title="Remove series"
            >
              {columnLabel(column)} x
            </button>
          ))}
        </div>
      )}
      {mixedUnits && (
        <p className="rounded-lg border border-yellow-500/40 bg-yellow-500/10 px-3 py-2 text-xs text-muted">
          Selected Y columns use different units. Consider separate graphs if scale comparison matters.
        </p>
      )}
      <button
        disabled={yColumns.length === 0}
        onClick={() =>
          onRun(
            { chart_type: chartType, x_column: xColumn || undefined, y_columns: yColumns, filters: [] },
            {
              xTitle: xColumn ? columnLabel(xColumn) : "Index",
              yTitle: yAxisTitle(),
              traceLabels: Object.fromEntries(yColumns.map((column) => [column, columnLabel(column)])),
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
