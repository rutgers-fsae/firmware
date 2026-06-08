export type FilterRule = {
  column: string;
  op: "eq" | "contains" | "gte" | "lte";
  value: string | number;
};

export type ChartType = "line" | "scatter" | "bar" | "histogram" | "box";

export type ChartRequest = {
  chart_type: ChartType;
  x_column?: string;
  y_columns: string[];
  group_by?: string;
  filters: FilterRule[];
};

export type ChartConfig = Pick<ChartRequest, "chart_type" | "x_column" | "y_columns">;

export type PlotTrace = Record<string, unknown>;
