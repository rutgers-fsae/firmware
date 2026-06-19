import { render, screen } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { describe, expect, it, vi } from "vitest";
import { ChartBuilder } from "./ChartBuilder";
import type { SchemaColumn } from "../types/dataset";

const columns: SchemaColumn[] = [
  { name: "Time", type: "numeric", unit: "s", display_name: "Time (s)", sample_values: ["0", "1"] },
  { name: "Speed", type: "numeric", unit: "mph", display_name: "Speed (mph)", sample_values: ["10", "20"] },
  { name: "Driver", type: "categorical", display_name: "Driver", sample_values: ["A"] },
];

describe("ChartBuilder", () => {
  it("emits selected chart request and axis titles", async () => {
    const user = userEvent.setup();
    const onRun = vi.fn();
    render(<ChartBuilder columns={columns} onRun={onRun} />);

    await user.click(screen.getByRole("button", { name: /select x axis/i }));
    await user.click(screen.getByRole("button", { name: "Time (s)" }));
    await user.click(screen.getByRole("button", { name: /select y series/i }));
    await user.click(screen.getByRole("checkbox", { name: "Speed (mph)" }));
    await user.click(screen.getByRole("button", { name: "Render" }));

    expect(onRun).toHaveBeenCalledWith(
      { chart_type: "line", x_column: "Time", y_columns: ["Speed"], filters: [] },
      expect.objectContaining({
        xTitle: "Time (s)",
        yTitle: "Speed (mph)",
      }),
    );
  });

  it("emits a time filter when time is not the x axis", async () => {
    const user = userEvent.setup();
    const onRun = vi.fn();
    render(<ChartBuilder columns={columns} onRun={onRun} />);

    await user.click(screen.getByRole("button", { name: /select x axis/i }));
    await user.click(screen.getByRole("button", { name: "Driver" }));
    await user.click(screen.getByRole("button", { name: /select y series/i }));
    await user.click(screen.getByRole("checkbox", { name: "Speed (mph)" }));
    await user.type(screen.getByRole("spinbutton", { name: "Time filter start" }), "10");
    await user.type(screen.getByRole("spinbutton", { name: "Time filter end" }), "20");
    await user.click(screen.getByRole("button", { name: "Render" }));

    expect(onRun).toHaveBeenCalledWith(
      {
        chart_type: "line",
        x_column: "Driver",
        y_columns: ["Speed"],
        filters: [
          { column: "Time", op: "gte", value: 10 },
          { column: "Time", op: "lte", value: 20 },
        ],
      },
      expect.objectContaining({
        xTitle: "Driver",
        yTitle: "Speed (mph)",
      }),
    );
  });

  it("shows an empty state when there are no numeric columns", () => {
    render(<ChartBuilder columns={[columns[2]]} onRun={vi.fn()} />);

    expect(screen.getByText("This dataset has no numeric columns to plot.")).toBeInTheDocument();
    expect(screen.getByRole("button", { name: "Render" })).toBeDisabled();
  });
});
