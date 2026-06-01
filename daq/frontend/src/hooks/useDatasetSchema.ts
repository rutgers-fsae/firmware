import { useEffect, useState } from "react";
import { getSchema } from "../api/datasets";
import type { SchemaColumn } from "../types/dataset";

export function useDatasetSchema(slug: string) {
  const [columns, setColumns] = useState<SchemaColumn[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    setLoading(true);
    setError(null);
    getSchema(slug)
      .then((result) => setColumns(result.columns))
      .catch((err: unknown) => {
        setColumns([]);
        setError(err instanceof Error ? err.message : "Failed to load dataset schema");
      })
      .finally(() => setLoading(false));
  }, [slug]);

  return { columns, loading, error };
}
