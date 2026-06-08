import { render, screen, waitFor } from "@testing-library/react";
import { act } from "react";
import { describe, expect, it, vi, beforeEach } from "vitest";
import { PlotView } from "./PlotView";

const plotState = vi.hoisted(() => ({
  lastProps: null as null | {
    data: Array<Record<string, unknown>>;
    layout: Record<string, unknown>;
    onError?: (error: Error) => void;
  },
}));

vi.mock("react-plotly.js/factory", () => ({
  default: vi.fn(() => (props: NonNullable<typeof plotState.lastProps>) => {
    plotState.lastProps = props;
    return <div data-testid="plot" />;
  }),
}));

vi.mock("plotly.js-cartesian-dist-min", () => ({
  default: {},
}));

const baseTrace = { name: "Speed", x: [0, 1], y: [10, 20], type: "scatter", mode: "lines" };

describe("PlotView", () => {
  beforeEach(() => {
    plotState.lastProps = null;
  });

  it("omits the secondary y axis for single-axis charts", async () => {
    render(
      <PlotView
        data={[baseTrace]}
        theme="light"
        axisTitles={{
          xTitle: "Time (s)",
          yTitle: "Speed (mph)",
          traceLabels: { Speed: "Speed (mph)" },
          traceAxisByColumn: { Speed: "y" },
        }}
      />,
    );

    await screen.findByTestId("plot");

    expect(plotState.lastProps?.layout).not.toHaveProperty("yaxis2");
    expect(plotState.lastProps?.data[0]).toMatchObject({ name: "Speed (mph)", yaxis: "y" });
  });

  it("includes the secondary y axis only for dual-axis charts", async () => {
    render(
      <PlotView
        data={[
          baseTrace,
          { ...baseTrace, name: "Voltage", y: [300, 310] },
        ]}
        theme="light"
        axisTitles={{
          xTitle: "Time (s)",
          yTitle: "Speed (mph)",
          y2Title: "Values (V)",
          traceLabels: { Speed: "Speed (mph)", Voltage: "Voltage (V)" },
          traceAxisByColumn: { Speed: "y", Voltage: "y2" },
        }}
      />,
    );

    await screen.findByTestId("plot");

    expect(plotState.lastProps?.layout).toHaveProperty("yaxis2");
    expect(plotState.lastProps?.data).toEqual(
      expect.arrayContaining([
        expect.objectContaining({ name: "Speed (mph)", yaxis: "y" }),
        expect.objectContaining({ name: "Voltage (V)", yaxis: "y2" }),
      ]),
    );
  });

  it("shows Plotly render errors instead of a blank plot", async () => {
    render(<PlotView data={[baseTrace]} theme="light" axisTitles={null} />);

    await screen.findByTestId("plot");
    act(() => {
      plotState.lastProps?.onError?.(new Error("Cannot read properties of undefined"));
    });

    await waitFor(() => {
      expect(screen.getByText(/Chart render failed: Cannot read properties of undefined/i)).toBeInTheDocument();
    });
  });
});
