import { Link, useNavigate } from "react-router-dom";
import { Database, FileSpreadsheet } from "lucide-react";
import { UploadPanel } from "../components/UploadPanel";
import { useDatasets } from "../hooks/useDatasets";
import { Alert, Badge, Panel } from "../components/ui";

function formatBytes(size: number) {
  if (!Number.isFinite(size) || size <= 0) return "0 B";
  const units = ["B", "KB", "MB", "GB"];
  const index = Math.min(Math.floor(Math.log(size) / Math.log(1024)), units.length - 1);
  return `${(size / 1024 ** index).toFixed(index === 0 ? 0 : 1)} ${units[index]}`;
}

export function DatasetListPage() {
  const { datasets, loading, error, refresh } = useDatasets();
  const navigate = useNavigate();

  return (
    <main className="grid gap-4 lg:grid-cols-[minmax(320px,420px)_1fr]">
      <UploadPanel
        onUploaded={(slug) => {
          refresh();
          navigate(`/datasets/${slug}`);
        }}
      />
      <Panel className="grid content-start gap-4 overflow-hidden">
        <div className="flex items-center justify-between gap-3 border-b border-border px-4 py-3">
          <div className="flex min-w-0 items-center gap-3">
            <span className="flex h-9 w-9 shrink-0 items-center justify-center rounded-md border border-border bg-surface-soft text-button">
              <Database size={17} aria-hidden="true" />
            </span>
            <div className="min-w-0">
              <h2 className="truncate text-base font-semibold">Datasets</h2>
              <p className="text-sm text-muted">CSV telemetry catalog</p>
            </div>
          </div>
          <Badge tone="default">{datasets.length} available</Badge>
        </div>
        <div className="grid gap-3 px-4 pb-4">
        {loading && <p className="text-sm text-muted">Loading datasets...</p>}
        {error && <Alert tone="danger">{error}</Alert>}
        {!loading && !error && datasets.length === 0 && <p className="text-sm text-muted">No datasets uploaded yet.</p>}
        <ul className="grid gap-2">
          {datasets.map((dataset) => (
            <li key={dataset.slug}>
              <Link
                to={`/datasets/${dataset.slug}`}
                className="group grid gap-2 rounded-lg border border-border bg-surface px-3 py-3 text-sm shadow-sm transition hover:-translate-y-0.5 hover:border-button hover:shadow-md"
              >
                <span className="flex items-start justify-between gap-3">
                  <span className="flex min-w-0 items-center gap-2">
                    <FileSpreadsheet size={16} aria-hidden="true" className="shrink-0 text-button" />
                    <span className="truncate font-semibold text-text group-hover:text-button">{dataset.title}</span>
                  </span>
                  <Badge tone="default" className="shrink-0">{formatBytes(dataset.size_bytes)}</Badge>
                </span>
                <span className="truncate text-xs text-muted">{dataset.filename}</span>
              </Link>
            </li>
          ))}
        </ul>
        </div>
      </Panel>
    </main>
  );
}
