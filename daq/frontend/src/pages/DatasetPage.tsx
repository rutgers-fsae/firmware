import { useCallback, useEffect, useMemo, useState } from "react";
import {
  DndContext,
  DragOverlay,
  KeyboardSensor,
  PointerSensor,
  closestCenter,
  type DragStartEvent,
  useSensor,
  useSensors,
  type DragEndEvent,
} from "@dnd-kit/core";
import {
  SortableContext,
  arrayMove,
  rectSortingStrategy,
  sortableKeyboardCoordinates,
  useSortable,
} from "@dnd-kit/sortable";
import { CSS } from "@dnd-kit/utilities";
import { GripVertical } from "lucide-react";
import { Link, useParams } from "react-router-dom";
import { ChartBuilder } from "../components/ChartBuilder";
import { PlotView } from "../components/PlotView";
import { getChartData } from "../api/datasets";
import { useDatasetSchema } from "../hooks/useDatasetSchema";
import type { ChartConfig, ChartRequest, PlotTrace } from "../types/chart";

type Props = {
  theme: "light" | "dark";
};

type GraphState = {
  id: number;
  name: string;
  chartConfig: ChartConfig;
  plotData: PlotTrace[];
  axisTitles: {
    xTitle: string;
    yTitle: string;
    y2Title?: string;
    traceLabels: Record<string, string>;
    traceAxisByColumn: Record<string, "y" | "y2">;
  } | null;
  chartError: string | null;
  isLoading: boolean;
};

type PersistedGraphState = Pick<GraphState, "id" | "name" | "chartConfig" | "axisTitles">;

function emptyGraph(id: number, name = `Graph ${id}`): GraphState {
  return {
    id,
    name,
    chartConfig: { chart_type: "line", y_columns: [] },
    plotData: [],
    axisTitles: null,
    chartError: null,
    isLoading: false,
  };
}

function restoreGraph(item: Partial<GraphState>, fallbackId: number): GraphState {
  const id = typeof item.id === "number" ? item.id : fallbackId;
  return {
    ...emptyGraph(id),
    name: typeof item.name === "string" ? item.name : `Graph ${id}`,
    chartConfig: restoreChartConfig(item.chartConfig),
    axisTitles: item.axisTitles || null,
  };
}

function graphForStorage(graph: GraphState): PersistedGraphState {
  return {
    id: graph.id,
    name: graph.name,
    chartConfig: graph.chartConfig,
    axisTitles: graph.axisTitles,
  };
}

function restoreChartConfig(config: Partial<ChartConfig> | undefined): ChartConfig {
  const chartType = config?.chart_type;
  return {
    chart_type:
      chartType === "line" || chartType === "scatter" || chartType === "bar" || chartType === "histogram" || chartType === "box"
        ? chartType
        : "line",
    x_column: typeof config?.x_column === "string" ? config.x_column : undefined,
    y_columns: Array.isArray(config?.y_columns) ? config.y_columns.filter((item): item is string => typeof item === "string") : [],
  };
}

function chartConfigsEqual(left: ChartConfig, right: ChartConfig): boolean {
  return (
    left.chart_type === right.chart_type &&
    left.x_column === right.x_column &&
    left.y_columns.length === right.y_columns.length &&
    left.y_columns.every((column, index) => column === right.y_columns[index])
  );
}

type SortableGraphCardProps = {
  graph: GraphState;
  index: number;
  totalGraphs: number;
  columns: Parameters<typeof ChartBuilder>[0]["columns"];
  theme: "light" | "dark";
  onRemove: (id: number) => void;
  onRename: (id: number, name: string) => void;
  onConfigChange: (id: number, config: ChartConfig) => void;
  onRun: (
    graphId: number,
    payload: ChartRequest,
    titles: {
      xTitle: string;
      yTitle: string;
      y2Title?: string;
      traceLabels: Record<string, string>;
      traceAxisByColumn: Record<string, "y" | "y2">;
    },
  ) => void;
};

function SortableGraphCard({
  graph,
  index,
  totalGraphs,
  columns,
  theme,
  onRemove,
  onRename,
  onConfigChange,
  onRun,
}: SortableGraphCardProps) {
  const { attributes, listeners, setNodeRef, transform, transition, isDragging } = useSortable({ id: graph.id });

  const style = {
    transform: CSS.Transform.toString(transform),
    transition,
  };

  const displayName = graph.name.trim() || `Graph ${index + 1}`;
  const handleConfigChange = useCallback((config: ChartConfig) => onConfigChange(graph.id, config), [graph.id, onConfigChange]);

  return (
    <article
      ref={setNodeRef}
      style={style}
      className={`grid gap-3 rounded-2xl border border-border bg-panel p-4 backdrop-blur ${isDragging ? "opacity-85 shadow-2xl" : ""}`}
    >
      <div className="flex items-center justify-between gap-2">
        <div className="flex items-center gap-2">
          <button
            type="button"
            className="cursor-grab rounded-lg border border-input-border bg-input p-1 text-muted active:cursor-grabbing"
            aria-label={`Reorder ${displayName}`}
            title={`Reorder ${displayName}`}
            {...attributes}
            {...listeners}
          >
            <GripVertical size={14} />
          </button>
          <input
            type="text"
            value={graph.name}
            onChange={(event) => onRename(graph.id, event.target.value)}
            className="w-full rounded-lg border border-input-border bg-input px-2 py-1 text-sm font-semibold text-text"
            aria-label={`Graph ${index + 1} name`}
            maxLength={60}
          />
        </div>
        <button
          type="button"
          onClick={() => onRemove(graph.id)}
          disabled={totalGraphs === 1}
          className="rounded-lg border border-input-border bg-input px-2 py-1 text-xs text-text transition hover:bg-button hover:text-button-text disabled:cursor-not-allowed disabled:opacity-50"
        >
          Remove
        </button>
      </div>
      <ChartBuilder
        columns={columns}
        config={graph.chartConfig}
        onConfigChange={handleConfigChange}
        onRun={(payload, titles) => onRun(graph.id, payload, titles)}
      />
      {graph.isLoading && <p className="text-sm text-muted">Rendering graph...</p>}
      {graph.chartError && (
        <p className="rounded-lg border border-red-500/40 bg-red-500/10 px-3 py-2 text-sm text-red-300">
          Chart load failed: {graph.chartError}
        </p>
      )}
      <div className="rounded-2xl border border-border bg-panel p-4">
        <PlotView data={graph.plotData} theme={theme} axisTitles={graph.axisTitles} />
      </div>
    </article>
  );
}

export function DatasetPage({ theme }: Props) {
  const { slug = "" } = useParams();
  const { columns, loading, error } = useDatasetSchema(slug);
  const [nextGraphId, setNextGraphId] = useState(2);
  const [graphs, setGraphs] = useState<GraphState[]>(() => {
    const key = `daq-graphs-${slug}`;
    const raw = localStorage.getItem(key);
    if (raw) {
      try {
        const parsed = JSON.parse(raw) as Partial<GraphState>[];
        if (Array.isArray(parsed) && parsed.length > 0) {
          return parsed.map((item, index) => restoreGraph(item, index + 1));
        }
      } catch {
        return [emptyGraph(1)];
      }
    }
    return [emptyGraph(1)];
  });
  const [activeGraphId, setActiveGraphId] = useState<number | null>(null);
  const [desktopLayout, setDesktopLayout] = useState<"one" | "two">(() => {
    const stored = localStorage.getItem("daq-graph-layout");
    return stored === "one" || stored === "two" ? stored : "two";
  });

  const sensors = useSensors(
    useSensor(PointerSensor),
    useSensor(KeyboardSensor, { coordinateGetter: sortableKeyboardCoordinates }),
  );

  function addGraph() {
    setGraphs((prev) => [
      ...prev,
      emptyGraph(nextGraphId),
    ]);
    setNextGraphId((prev) => prev + 1);
  }

  function removeGraph(id: number) {
    setGraphs((prev) => (prev.length > 1 ? prev.filter((graph) => graph.id !== id) : prev));
  }

  function handleDragStart(event: DragStartEvent) {
    setActiveGraphId(Number(event.active.id));
  }

  function handleDragEnd(event: DragEndEvent) {
    const { active, over } = event;
    setActiveGraphId(null);
    if (!over || active.id === over.id) {
      return;
    }
    setGraphs((prev) => {
      const oldIndex = prev.findIndex((graph) => graph.id === active.id);
      const newIndex = prev.findIndex((graph) => graph.id === over.id);
      if (oldIndex === -1 || newIndex === -1) {
        return prev;
      }
      return arrayMove(prev, oldIndex, newIndex);
    });
  }

  function handleDragCancel() {
    setActiveGraphId(null);
  }

  function setLayout(mode: "one" | "two") {
    setDesktopLayout(mode);
    localStorage.setItem("daq-graph-layout", mode);
  }

  const activeGraph = useMemo(() => graphs.find((graph) => graph.id === activeGraphId) ?? null, [graphs, activeGraphId]);

  function renameGraph(id: number, name: string) {
    setGraphs((prev) => prev.map((graph) => (graph.id === id ? { ...graph, name } : graph)));
  }

  const updateGraphConfig = useCallback((id: number, chartConfig: ChartConfig) => {
    setGraphs((prev) =>
      prev.map((graph) => {
        if (graph.id !== id || chartConfigsEqual(graph.chartConfig, chartConfig)) {
          return graph;
        }
        return { ...graph, chartConfig };
      }),
    );
  }, []);

  useEffect(() => {
    const maxGraphId = graphs.reduce((acc, graph) => Math.max(acc, graph.id), 1);
    setNextGraphId(maxGraphId + 1);
    localStorage.setItem(`daq-graphs-${slug}`, JSON.stringify(graphs.map(graphForStorage)));
  }, [graphs, slug]);

  const graphGridClass = `grid grid-cols-1 gap-4 ${desktopLayout === "one" ? "lg:grid-cols-1" : "lg:grid-cols-2"}`;

  async function runChart(
    graphId: number,
    payload: ChartRequest,
    titles: {
      xTitle: string;
      yTitle: string;
      y2Title?: string;
      traceLabels: Record<string, string>;
      traceAxisByColumn: Record<string, "y" | "y2">;
    },
  ) {
    setGraphs((prev) =>
      prev.map((graph) =>
        graph.id === graphId ? { ...graph, chartError: null, isLoading: true } : graph,
      ),
    );
    try {
      const result = await getChartData(slug, payload);
      setGraphs((prev) =>
        prev.map((graph) =>
          graph.id === graphId
            ? {
                ...graph,
                plotData: result.data as PlotTrace[],
                axisTitles: titles,
                chartError: null,
                isLoading: false,
              }
            : graph,
        ),
      );
    } catch (err: unknown) {
      setGraphs((prev) =>
        prev.map((graph) =>
          graph.id === graphId
            ? {
                ...graph,
                chartError: err instanceof Error ? err.message : "Failed to load chart data",
                plotData: [],
                axisTitles: null,
                isLoading: false,
              }
            : graph,
        ),
      );
    }
  }

  return (
    <main className="grid gap-4">
      <p>
        <Link to="/" className="text-sm font-medium text-text hover:underline">
          Back to datasets
        </Link>
      </p>
      <h2 className="text-xl font-semibold">Dataset: {slug}</h2>
      <div>
        <div className="flex flex-wrap items-center gap-2">
          <button
            type="button"
            onClick={addGraph}
            className="rounded-lg bg-button px-3 py-2 text-sm font-medium text-button-text transition hover:bg-button-hover"
          >
            Add New Graph
          </button>
          <div className="hidden items-center gap-1 rounded-lg border border-input-border bg-input p-1 lg:flex">
            <button
              type="button"
              onClick={() => setLayout("one")}
              className={`rounded-md px-2 py-1 text-xs transition ${
                desktopLayout === "one" ? "bg-button text-button-text" : "text-text hover:bg-button hover:text-button-text"
              }`}
            >
              1 Column
            </button>
            <button
              type="button"
              onClick={() => setLayout("two")}
              className={`rounded-md px-2 py-1 text-xs transition ${
                desktopLayout === "two" ? "bg-button text-button-text" : "text-text hover:bg-button hover:text-button-text"
              }`}
            >
              2 Columns
            </button>
          </div>
        </div>
      </div>
      {loading && <p className="text-sm text-muted">Loading schema...</p>}
      {error && <p className="rounded-lg border border-red-500/40 bg-red-500/10 px-3 py-2 text-sm text-red-300">Schema load failed: {error}</p>}
      {!loading && !error && (
        <DndContext
          sensors={sensors}
          collisionDetection={closestCenter}
          onDragStart={handleDragStart}
          onDragEnd={handleDragEnd}
          onDragCancel={handleDragCancel}
        >
          <SortableContext items={graphs.map((graph) => graph.id)} strategy={rectSortingStrategy}>
            <section className={graphGridClass}>
              {graphs.map((graph, index) => (
                <SortableGraphCard
                  key={graph.id}
                  graph={graph}
                  index={index}
                  totalGraphs={graphs.length}
                  columns={columns}
                  theme={theme}
                  onRemove={removeGraph}
                  onRename={renameGraph}
                  onConfigChange={updateGraphConfig}
                  onRun={runChart}
                />
              ))}
            </section>
          </SortableContext>
          <DragOverlay>
            {activeGraph ? (
              <article className="grid min-w-[300px] gap-2 rounded-2xl border border-border bg-panel p-4 shadow-2xl backdrop-blur">
                <h3 className="text-base font-semibold">{activeGraph.name.trim() || `Graph ${graphs.findIndex((g) => g.id === activeGraph.id) + 1}`}</h3>
                <p className="text-sm text-muted">Dragging graph card...</p>
              </article>
            ) : null}
          </DragOverlay>
        </DndContext>
      )}
    </main>
  );
}
