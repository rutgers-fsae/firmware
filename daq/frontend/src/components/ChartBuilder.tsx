import { useMemo, useState } from "react";
import type { SchemaColumn } from "../types/dataset";
import type { ChartRequest } from "../types/chart";

type Props = {
  columns: SchemaColumn[];
  onRun: (payload: ChartRequest) => void;
};

export function ChartBuilder({ columns, onRun }: Props) {
  const [chartType, setChartType] = useState<ChartRequest["chart_type"]>("line");
  const [xColumn, setXColumn] = useState("");
  const [yColumn, setYColumn] = useState("");

  const numericColumns = useMemo(() => columns.filter((col) => col.type === "numeric"), [columns]);

  return (
    <section className="panel">
      <h3>Chart Builder</h3>
      <select value={chartType} onChange={(e) => setChartType(e.target.value as ChartRequest["chart_type"])}>
        <option value="line">Line</option>
        <option value="scatter">Scatter</option>
        <option value="bar">Bar</option>
        <option value="histogram">Histogram</option>
        <option value="box">Box</option>
      </select>
      <select value={xColumn} onChange={(e) => setXColumn(e.target.value)}>
        <option value="">X Axis</option>
        {columns.map((col) => (
          <option key={col.name} value={col.name}>{col.name}</option>
        ))}
      </select>
      <select value={yColumn} onChange={(e) => setYColumn(e.target.value)}>
        <option value="">Y Axis</option>
        {numericColumns.map((col) => (
          <option key={col.name} value={col.name}>{col.name}</option>
        ))}
      </select>
      <button disabled={!yColumn} onClick={() => onRun({ chart_type: chartType, x_column: xColumn || undefined, y_columns: [yColumn], filters: [] })}>
        Render
      </button>
    </section>
  );
}
