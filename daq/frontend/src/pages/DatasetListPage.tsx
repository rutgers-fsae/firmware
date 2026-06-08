import { Link, useNavigate } from "react-router-dom";
import { UploadPanel } from "../components/UploadPanel";
import { useDatasets } from "../hooks/useDatasets";
import { Alert, Panel } from "../components/ui";

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
      <Panel className="grid content-start gap-3 p-4">
        <div className="flex items-center justify-between gap-3">
          <div>
            <h2 className="text-base font-semibold">Datasets</h2>
            <p className="text-sm text-muted">{datasets.length} available</p>
          </div>
        </div>
        {loading && <p className="text-sm text-muted">Loading...</p>}
        {error && <Alert tone="danger">{error}</Alert>}
        {!loading && !error && datasets.length === 0 && <p className="text-sm text-muted">No datasets uploaded yet.</p>}
        <ul className="grid gap-2">
          {datasets.map((dataset) => (
            <li key={dataset.slug} className="rounded-md border border-border bg-surface-soft px-3 py-2 text-sm transition hover:border-input-border hover:bg-input">
              <Link to={`/datasets/${dataset.slug}`} className="font-medium text-text hover:underline">
                {dataset.title}
              </Link>
              <p className="mt-1 truncate text-xs text-muted">{dataset.filename}</p>
            </li>
          ))}
        </ul>
      </Panel>
    </main>
  );
}
