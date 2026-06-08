declare module "react-plotly.js" {
  import type { ComponentType } from "react";

  const Plot: ComponentType<Record<string, unknown>>;
  export default Plot;
}

declare module "react-plotly.js/factory" {
  import type { ComponentType } from "react";

  export default function createPlotlyComponent(plotly: unknown): ComponentType<Record<string, unknown>>;
}

declare module "plotly.js-cartesian-dist-min" {
  const Plotly: unknown;
  export default Plotly;
}
