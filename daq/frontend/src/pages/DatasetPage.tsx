import { useState } from "react";
import { Link, useParams } from "react-router-dom";
import { ChartBuilder } from "../components/ChartBuilder";
import { PlotView } from "../components/PlotView";
import { getChartData } from "../api/datasets";
import { useDatasetSchema } from "../hooks/useDatasetSchema";
import type { ChartRequest } from "../types/chart";

type Props = {
  theme: "light" | "dark";
};

export function DatasetPage({ theme }: Props) {
  const { slug = "" } = useParams();
  const { columns, loading, error } = useDatasetSchema(slug);
  const [plotData, setPlotData] = useState<any[]>([]);
  const [chartError, setChartError] = useState<string | null>(null);

  async function runChart(payload: ChartRequest) {
    setChartError(null);
    try {
      const result = await getChartData(slug, payload);
      setPlotData(result.data as any[]);
    } catch (err: unknown) {
      setChartError(err instanceof Error ? err.message : "Failed to load chart data");
      setPlotData([]);
    }
  }

  return (
    <main>
      <p><Link to="/">Back to datasets</Link></p>
      <h2>Dataset: {slug}</h2>
      {loading ? <p>Loading schema...</p> : <ChartBuilder columns={columns} onRun={runChart} />}
      {error && <p>Schema load failed: {error}</p>}
      {chartError && <p>Chart load failed: {chartError}</p>}
      <section className="panel">
        <PlotView data={plotData} theme={theme} />
      </section>
    </main>
  );
}
