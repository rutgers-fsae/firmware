import { useEffect, useMemo, useRef, useState } from "react";
import { createPortal } from "react-dom";
import type { SchemaColumn } from "../types/dataset";
import type { ChartConfig, ChartRequest } from "../types/chart";

type Props = {
  columns: SchemaColumn[];
  config?: ChartConfig;
  onConfigChange?: (config: ChartConfig) => void;
  onRun: (
    payload: ChartRequest,
    axisTitles: {
      xTitle: string;
      yTitle: string;
      y2Title?: string;
      traceLabels: Record<string, string>;
      traceAxisByColumn: Record<string, "y" | "y2">;
    },
  ) => void;
};

type DropdownPosition = {
  top: number;
  left: number;
  width: number;
};

const defaultConfig: ChartConfig = { chart_type: "line", y_columns: [] };

export function ChartBuilder({ columns, config = defaultConfig, onConfigChange, onRun }: Props) {
  const [chartType, setChartType] = useState<ChartRequest["chart_type"]>(config.chart_type);
  const [xColumn, setXColumn] = useState(config.x_column || "");
  const [yColumns, setYColumns] = useState<string[]>(config.y_columns);
  const [isXDropdownOpen, setIsXDropdownOpen] = useState(false);
  const [isYDropdownOpen, setIsYDropdownOpen] = useState(false);
  const xDropdownRef = useRef<HTMLDivElement | null>(null);
  const yDropdownRef = useRef<HTMLDivElement | null>(null);
  const xButtonRef = useRef<HTMLButtonElement | null>(null);
  const yButtonRef = useRef<HTMLButtonElement | null>(null);
  const xPanelRef = useRef<HTMLDivElement | null>(null);
  const yPanelRef = useRef<HTMLDivElement | null>(null);
  const [xDropdownPosition, setXDropdownPosition] = useState<DropdownPosition | null>(null);
  const [yDropdownPosition, setYDropdownPosition] = useState<DropdownPosition | null>(null);

  const numericColumns = useMemo(() => columns.filter((col) => col.type === "numeric"), [columns]);

  useEffect(() => {
    onConfigChange?.({ chart_type: chartType, x_column: xColumn || undefined, y_columns: yColumns });
  }, [chartType, onConfigChange, xColumn, yColumns]);

  useEffect(() => {
    const available = new Set(columns.map((column) => column.name));
    setXColumn((current) => (current && !available.has(current) ? "" : current));
    setYColumns((current) => current.filter((column) => available.has(column)));
  }, [columns]);

  useEffect(() => {
    function closeOnOutsideClick(event: MouseEvent) {
      const target = event.target as Node;
      if (
        xDropdownRef.current &&
        !xDropdownRef.current.contains(target) &&
        xPanelRef.current &&
        !xPanelRef.current.contains(target)
      ) {
        setIsXDropdownOpen(false);
      }
      if (
        yDropdownRef.current &&
        !yDropdownRef.current.contains(target) &&
        yPanelRef.current &&
        !yPanelRef.current.contains(target)
      ) {
        setIsYDropdownOpen(false);
      }
    }

    document.addEventListener("mousedown", closeOnOutsideClick);
    return () => document.removeEventListener("mousedown", closeOnOutsideClick);
  }, []);

  useEffect(() => {
    function updateDropdownPositions() {
      if (isXDropdownOpen) {
        setXDropdownPosition(positionForButton(xButtonRef.current));
      }
      if (isYDropdownOpen) {
        setYDropdownPosition(positionForButton(yButtonRef.current));
      }
    }

    window.addEventListener("resize", updateDropdownPositions);
    window.addEventListener("scroll", updateDropdownPositions, true);
    return () => {
      window.removeEventListener("resize", updateDropdownPositions);
      window.removeEventListener("scroll", updateDropdownPositions, true);
    };
  }, [isXDropdownOpen, isYDropdownOpen]);

  function positionForButton(button: HTMLButtonElement | null): DropdownPosition | null {
    if (!button) return null;
    const rect = button.getBoundingClientRect();
    return {
      top: rect.bottom + 8,
      left: rect.left,
      width: rect.width,
    };
  }

  function toggleXDropdown() {
    setIsXDropdownOpen((open) => {
      const next = !open;
      if (next) {
        setXDropdownPosition(positionForButton(xButtonRef.current));
      }
      return next;
    });
  }

  function toggleYDropdown() {
    setIsYDropdownOpen((open) => {
      const next = !open;
      if (next) {
        setYDropdownPosition(positionForButton(yButtonRef.current));
      }
      return next;
    });
  }

  function columnLabel(columnName: string) {
    const column = columns.find((item) => item.name === columnName);
    if (!column) return columnName;
    return column.display_name || column.name;
  }

  function columnUnit(columnName: string) {
    return columns.find((item) => item.name === columnName)?.unit || null;
  }

  function toggleYColumn(columnName: string) {
    setYColumns((prev) =>
      prev.includes(columnName) ? prev.filter((item) => item !== columnName) : [...prev, columnName],
    );
  }

  const selectedUnits = Array.from(new Set(yColumns.map((column) => columnUnit(column) || "unitless")));
  const hasMultipleUnits = selectedUnits.length > 1;
  const hasTooManyUnits = selectedUnits.length > 2;
  const primaryUnit = selectedUnits[0];
  const secondaryUnit = selectedUnits[1];
  const primaryUnitLabel = primaryUnit === "unitless" ? "Unitless" : primaryUnit;
  const secondaryUnitLabel = secondaryUnit === "unitless" ? "Unitless" : secondaryUnit;

  function axisTitleForUnit(unit: string | undefined) {
    if (!unit) return "Values";
    return unit === "unitless" ? "Values" : `Values (${unit})`;
  }

  function traceAxisByColumn() {
    return Object.fromEntries(
      yColumns.map((column) => [column, (columnUnit(column) || "unitless") === primaryUnit ? "y" : "y2"]),
    ) as Record<string, "y" | "y2">;
  }
  const yDropdownLabel =
    yColumns.length === 0 ? "Select Y Series" : yColumns.length === 1 ? "1 series selected" : `${yColumns.length} series selected`;
  const xDropdownLabel = xColumn ? columnLabel(xColumn) : "Select X Axis";

  return (
    <section className="grid gap-3 rounded-2xl border border-border bg-panel p-4 backdrop-blur">
      <h3 className="text-lg font-semibold">Chart Builder</h3>
      <select
        value={chartType}
        onChange={(e) => setChartType(e.target.value as ChartRequest["chart_type"])}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      >
        <option value="line">Line</option>
        <option value="scatter">Scatter</option>
        <option value="bar">Bar</option>
        <option value="histogram">Histogram</option>
        <option value="box">Box</option>
      </select>
      {numericColumns.length === 0 && (
        <p className="rounded-lg border border-yellow-500/40 bg-yellow-500/10 px-3 py-2 text-xs text-muted">
          This dataset has no numeric columns to plot.
        </p>
      )}
      <div ref={xDropdownRef} className="relative">
        <button
          ref={xButtonRef}
          type="button"
          onClick={toggleXDropdown}
          className="flex w-full items-center justify-between rounded-lg border border-input-border bg-input px-3 py-2 text-left text-sm text-text"
          aria-expanded={isXDropdownOpen}
        >
          <span>{xDropdownLabel}</span>
          <span aria-hidden="true" className="text-muted">▾</span>
        </button>
        {isXDropdownOpen && xDropdownPosition && createPortal(
          <div
            ref={xPanelRef}
            style={{ top: xDropdownPosition.top, left: xDropdownPosition.left, width: xDropdownPosition.width }}
            className="fixed z-[9999] grid max-h-64 gap-1 overflow-y-auto rounded-lg border border-border bg-panel p-3 shadow-2xl backdrop-blur"
          >
            <button
              type="button"
              onClick={() => {
                setXColumn("");
                setIsXDropdownOpen(false);
              }}
              className="rounded-md px-2 py-1 text-left text-sm text-muted transition hover:bg-input hover:text-text"
            >
              Use row index
            </button>
            {columns.map((col) => (
              <button
                key={col.name}
                type="button"
                onClick={() => {
                  setXColumn(col.name);
                  setIsXDropdownOpen(false);
                }}
                className={`rounded-md px-2 py-1 text-left text-sm transition hover:bg-input hover:text-text ${
                  xColumn === col.name ? "bg-input text-text" : "text-text"
                }`}
              >
                {col.display_name || col.name}
              </button>
            ))}
          </div>,
          document.body,
        )}
      </div>
      <div ref={yDropdownRef} className="relative">
        <button
          ref={yButtonRef}
          type="button"
          onClick={toggleYDropdown}
          className="flex w-full items-center justify-between rounded-lg border border-input-border bg-input px-3 py-2 text-left text-sm text-text"
          aria-expanded={isYDropdownOpen}
        >
          <span>{yDropdownLabel}</span>
          <span aria-hidden="true" className="text-muted">▾</span>
        </button>
        {isYDropdownOpen && yDropdownPosition && createPortal(
          <div
            ref={yPanelRef}
            style={{ top: yDropdownPosition.top, left: yDropdownPosition.left, width: yDropdownPosition.width }}
            className="fixed z-[9999] grid max-h-64 gap-1 overflow-y-auto rounded-lg border border-border bg-panel p-3 shadow-2xl backdrop-blur"
          >
            <div className="mb-1 flex items-center justify-between gap-2">
              <p className="text-sm font-medium text-text">Y Axis Series</p>
              {yColumns.length > 0 && (
                <button
                  type="button"
                  onClick={() => setYColumns([])}
                  className="rounded-md px-2 py-1 text-xs text-muted transition hover:bg-input hover:text-text"
                >
                  Clear all
                </button>
              )}
            </div>
            {numericColumns.map((col) => (
              <label key={col.name} className="flex items-center gap-2 rounded-md px-2 py-1 text-sm text-text hover:bg-input">
                <input
                  type="checkbox"
                  checked={yColumns.includes(col.name)}
                  onChange={() => toggleYColumn(col.name)}
                  className="h-4 w-4 accent-button"
                />
                <span>{col.display_name || col.name}</span>
              </label>
            ))}
          </div>,
          document.body,
        )}
      </div>
      {yColumns.length > 0 && (
        <div className="flex flex-wrap gap-2">
          {yColumns.map((column) => (
            <button
              key={column}
              type="button"
              onClick={() => toggleYColumn(column)}
              className="rounded-full border border-input-border bg-input px-2 py-1 text-xs text-text transition hover:bg-button hover:text-button-text"
              title="Remove series"
            >
              {columnLabel(column)} x
            </button>
          ))}
        </div>
      )}
      {hasMultipleUnits && !hasTooManyUnits && (
        <p className="rounded-lg border border-blue-500/40 bg-blue-500/10 px-3 py-2 text-xs text-muted">
          Using dual Y-axes for {primaryUnitLabel} and {secondaryUnitLabel}.
        </p>
      )}
      {hasTooManyUnits && (
        <p className="rounded-lg border border-yellow-500/40 bg-yellow-500/10 px-3 py-2 text-xs text-muted">
          More than two Y-axis units selected. Only two axes are supported per graph.
        </p>
      )}
      <button
        disabled={yColumns.length === 0 || hasTooManyUnits}
        onClick={() =>
          onRun(
            { chart_type: chartType, x_column: xColumn || undefined, y_columns: yColumns, filters: [] },
            {
              xTitle: xColumn ? columnLabel(xColumn) : "Index",
              yTitle: yColumns.length === 1 ? columnLabel(yColumns[0]) : axisTitleForUnit(primaryUnit),
              y2Title: secondaryUnit ? axisTitleForUnit(secondaryUnit) : undefined,
              traceLabels: Object.fromEntries(yColumns.map((column) => [column, columnLabel(column)])),
              traceAxisByColumn: traceAxisByColumn(),
            },
          )
        }
        className="rounded-lg bg-button px-3 py-2 text-sm font-medium text-button-text transition hover:bg-button-hover disabled:cursor-not-allowed disabled:bg-button-disabled"
      >
        Render
      </button>
    </section>
  );
}
