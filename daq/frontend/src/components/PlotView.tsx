import { useEffect, useState } from "react";
import type { ComponentType, CSSProperties } from "react";
import type { PlotTrace } from "../types/chart";

type Props = {
  data: PlotTrace[];
  theme: "light" | "dark";
  axisTitles: {
    xTitle: string;
    yTitle: string;
    y2Title?: string;
    traceLabels: Record<string, string>;
    traceAxisByColumn: Record<string, "y" | "y2">;
  } | null;
};

type PlotComponentProps = {
  data: PlotTrace[];
  layout: Record<string, unknown>;
  style: CSSProperties;
};

export function PlotView({ data, theme, axisTitles }: Props) {
  const [PlotComponent, setPlotComponent] = useState<ComponentType<PlotComponentProps> | null>(null);

  useEffect(() => {
    let mounted = true;
    Promise.all([import("react-plotly.js/factory"), import("plotly.js-cartesian-dist-min")]).then(([factory, plotly]) => {
      if (mounted) {
        const component = factory.default(plotly.default);
        setPlotComponent(() => component as ComponentType<PlotComponentProps>);
      }
    });
    return () => {
      mounted = false;
    };
  }, []);

  if (!data.length) return <p className="p-6 text-sm text-muted">No chart data yet.</p>;
  if (!PlotComponent) return <p className="p-6 text-sm text-muted">Loading chart engine...</p>;
  const dark = theme === "dark";
  const colors = dark
    ? { paper: "#121821", plot: "#121821", text: "#e7edf5", grid: "#2b3544", zero: "#465466" }
    : { paper: "#ffffff", plot: "#ffffff", text: "#17212f", grid: "#d9e0e8", zero: "#aeb8c4" };
  const traceLabels = axisTitles?.traceLabels || {};
  const traceAxisByColumn = axisTitles?.traceAxisByColumn || {};
  const labeledData = data.map((trace) => {
    const name = typeof trace.name === "string" ? trace.name : "";
    const directLabel = traceLabels[name];
    const groupedLabel = Object.entries(traceLabels).find(([column]) => name.endsWith(` - ${column}`));
    const sourceColumn = directLabel ? name : groupedLabel?.[0];
    return {
      ...trace,
      name: directLabel || (groupedLabel ? name.replace(` - ${groupedLabel[0]}`, ` - ${groupedLabel[1]}`) : name),
      yaxis: sourceColumn ? traceAxisByColumn[sourceColumn] : undefined,
    };
  });
  const hasRightAxis = Boolean(axisTitles?.y2Title);
  return (
    <PlotComponent
      data={labeledData}
      layout={{
        autosize: true,
        title: "Dataset Graph",
        margin: { l: 64, r: hasRightAxis ? 64 : 24, t: 48, b: 56 },
        xaxis: { title: { text: axisTitles?.xTitle || "" }, gridcolor: colors.grid, zerolinecolor: colors.zero },
        yaxis: { title: { text: axisTitles?.yTitle || "" }, gridcolor: colors.grid, zerolinecolor: colors.zero },
        yaxis2: hasRightAxis
          ? {
              title: { text: axisTitles?.y2Title || "" },
              overlaying: "y",
              side: "right",
              gridcolor: colors.grid,
              zerolinecolor: colors.zero,
            }
          : undefined,
        paper_bgcolor: colors.paper,
        plot_bgcolor: colors.plot,
        font: { color: colors.text, family: "Inter, ui-sans-serif, system-ui, sans-serif" },
      }}
      style={{ width: "100%", height: "520px" }}
    />
  );
}
