import { Link, useNavigate } from "react-router-dom";
import { UploadPanel } from "../components/UploadPanel";
import { useDatasets } from "../hooks/useDatasets";

export function DatasetListPage() {
  const { datasets, loading, refresh } = useDatasets();
  const navigate = useNavigate();

  return (
    <main>
      <UploadPanel
        onUploaded={(slug) => {
          refresh();
          navigate(`/datasets/${slug}`);
        }}
      />
      <section className="panel">
        <h2>Datasets</h2>
        {loading && <p>Loading...</p>}
        <ul>
          {datasets.map((dataset) => (
            <li key={dataset.slug}>
              <Link to={`/datasets/${dataset.slug}`}>{dataset.title}</Link> ({dataset.filename})
            </li>
          ))}
        </ul>
      </section>
    </main>
  );
}
