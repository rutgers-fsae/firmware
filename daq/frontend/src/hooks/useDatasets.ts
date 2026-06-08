import { useEffect, useState } from "react";
import { listDatasets } from "../api/datasets";
import type { Dataset } from "../types/dataset";

export function useDatasets() {
  const [datasets, setDatasets] = useState<Dataset[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  function refresh() {
    setLoading(true);
    setError(null);
    return listDatasets()
      .then(setDatasets)
      .catch((err: unknown) => {
        setDatasets([]);
        setError(err instanceof Error ? err.message : "Failed to load datasets");
      })
      .finally(() => setLoading(false));
  }

  useEffect(() => {
    refresh();
  }, []);

  return { datasets, loading, error, refresh };
}
