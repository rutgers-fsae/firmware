export type FilterRule = {
  column: string;
  op: string;
  value: string | number;
};

export type ChartRequest = {
  chart_type: "line" | "scatter" | "bar" | "histogram" | "box";
  x_column?: string;
  y_columns: string[];
  group_by?: string;
  filters: FilterRule[];
};
