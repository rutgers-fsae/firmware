import { useEffect, useState } from "react";

type Props = {
  data: any[];
  theme: "light" | "dark";
  axisTitles: { xTitle: string; yTitle: string; traceLabels: Record<string, string> } | null;
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
  const labeledData = data.map((trace) => {
    const name = typeof trace.name === "string" ? trace.name : "";
    const directLabel = traceLabels[name];
    const groupedLabel = Object.entries(traceLabels).find(([column]) => name.endsWith(` - ${column}`));
    return {
      ...trace,
      name: directLabel || (groupedLabel ? name.replace(` - ${groupedLabel[0]}`, ` - ${groupedLabel[1]}`) : name),
    };
  });
  return (
    <PlotComponent
      data={labeledData}
      layout={{
        autosize: true,
        title: "Dataset Graph",
        xaxis: { title: { text: axisTitles?.xTitle || "" } },
        yaxis: { title: { text: axisTitles?.yTitle || "" } },
        paper_bgcolor: dark ? "#0f1722" : "#ffffff",
        plot_bgcolor: dark ? "#0f1722" : "#ffffff",
        font: { color: dark ? "#e2ecff" : "#102031" },
      }}
      style={{ width: "100%", height: "540px" }}
    />
  );
}
