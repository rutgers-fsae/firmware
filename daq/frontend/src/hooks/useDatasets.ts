import { useEffect, useState } from "react";
import { listDatasets } from "../api/datasets";
import type { Dataset } from "../types/dataset";

export function useDatasets() {
  const [datasets, setDatasets] = useState<Dataset[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    listDatasets()
      .then(setDatasets)
      .finally(() => setLoading(false));
  }, []);

  return { datasets, loading, refresh: () => listDatasets().then(setDatasets) };
}
