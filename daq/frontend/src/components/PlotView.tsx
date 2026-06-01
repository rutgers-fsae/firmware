import Plot from "react-plotly.js";

type Props = {
  data: any[];
  theme: "light" | "dark";
};

export function PlotView({ data, theme }: Props) {
  if (!data.length) return <p>No chart data yet.</p>;
  const dark = theme === "dark";
  return (
    <Plot
      data={data}
      layout={{
        autosize: true,
        title: "Dataset Graph",
        paper_bgcolor: dark ? "#0f1722" : "#ffffff",
        plot_bgcolor: dark ? "#0f1722" : "#ffffff",
        font: { color: dark ? "#e2ecff" : "#102031" },
      }}
      style={{ width: "100%", height: "540px" }}
    />
  );
}
