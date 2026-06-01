import { Link, useNavigate } from "react-router-dom";
import { UploadPanel } from "../components/UploadPanel";
import { useDatasets } from "../hooks/useDatasets";

export function DatasetListPage() {
  const { datasets, loading, refresh } = useDatasets();
  const navigate = useNavigate();

  return (
    <main className="grid gap-4">
      <UploadPanel
        onUploaded={(slug) => {
          refresh();
          navigate(`/datasets/${slug}`);
        }}
      />
      <section className="grid gap-3 rounded-2xl border border-border bg-panel p-4 backdrop-blur">
        <h2 className="text-lg font-semibold">Datasets</h2>
        {loading && <p className="text-sm text-muted">Loading...</p>}
        <ul className="grid gap-2">
          {datasets.map((dataset) => (
            <li key={dataset.slug} className="rounded-lg border border-border/70 bg-input/70 px-3 py-2 text-sm">
              <Link to={`/datasets/${dataset.slug}`} className="font-medium text-text hover:underline">
                {dataset.title}
              </Link>{" "}
              <span className="text-muted">({dataset.filename})</span>
            </li>
          ))}
        </ul>
      </section>
    </main>
  );
}
