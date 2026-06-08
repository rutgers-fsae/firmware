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

  if (!data.length) return <p>No chart data yet.</p>;
  if (!PlotComponent) return <p>Loading chart engine...</p>;
  const dark = theme === "dark";
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
        xaxis: { title: { text: axisTitles?.xTitle || "" } },
        yaxis: { title: { text: axisTitles?.yTitle || "" } },
        yaxis2: hasRightAxis
          ? {
              title: { text: axisTitles?.y2Title || "" },
              overlaying: "y",
              side: "right",
            }
          : undefined,
        paper_bgcolor: dark ? "#0f1722" : "#ffffff",
        plot_bgcolor: dark ? "#0f1722" : "#ffffff",
        font: { color: dark ? "#e2ecff" : "#102031" },
      }}
      style={{ width: "100%", height: "540px" }}
    />
  );
}
