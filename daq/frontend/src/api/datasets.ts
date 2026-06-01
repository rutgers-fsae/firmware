import { apiFetch } from "./client";
import type { ChartRequest } from "../types/chart";
import type { Dataset, SchemaColumn } from "../types/dataset";

export function listDatasets() {
  return apiFetch<Dataset[]>("/api/datasets");
}

export function getSchema(slug: string) {
  return apiFetch<{ columns: SchemaColumn[]; row_count: number }>(`/api/datasets/${slug}/schema`);
}

export function getChartData(slug: string, payload: ChartRequest) {
  return apiFetch<{ data: unknown[]; row_count: number }>(`/api/datasets/${slug}/chart-data`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
}

export async function uploadDataset(file: File, password: string) {
  const form = new FormData();
  form.append("file", file);
  return apiFetch<{ slug: string; title: string; filename: string }>("/api/upload", {
    method: "POST",
    headers: { Authorization: `Bearer ${password}` },
    body: form,
  });
}
