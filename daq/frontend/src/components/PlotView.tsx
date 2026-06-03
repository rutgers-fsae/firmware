import { useEffect, useState } from "react";

type Props = {
  data: any[];
  theme: "light" | "dark";
  axisTitles: {
    xTitle: string;
    yTitle: string;
    y2Title?: string;
    traceLabels: Record<string, string>;
    traceAxisByColumn: Record<string, "y" | "y2">;
  } | null;
};

export function PlotView({ data, theme, axisTitles }: Props) {
  const [PlotComponent, setPlotComponent] = useState<any>(null);

  useEffect(() => {
    let mounted = true;
    import("react-plotly.js").then((mod) => {
      if (mounted) {
        setPlotComponent(() => mod.default);
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
