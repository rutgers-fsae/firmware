import { useState } from "react";
import { uploadDataset } from "../api/datasets";

type Props = {
  onUploaded: (slug: string) => void;
};

export function UploadPanel({ onUploaded }: Props) {
  const [password, setPassword] = useState("");
  const [file, setFile] = useState<File | null>(null);
  const [status, setStatus] = useState("");
  const [isUploading, setIsUploading] = useState(false);

  async function submit() {
    if (!file || isUploading) return;
    setIsUploading(true);
    setStatus("");
    try {
      const result = await uploadDataset(file, password);
      setStatus(`Uploaded ${result.filename}`);
      setFile(null);
      onUploaded(result.slug);
    } catch (error) {
      setStatus(error instanceof Error ? error.message : "Upload failed");
    } finally {
      setIsUploading(false);
    }
  }

  return (
    <section className="grid gap-3 rounded-2xl border border-border bg-panel p-4 backdrop-blur">
      <h3 className="text-lg font-semibold">Upload CSV</h3>
      <input
        type="password"
        placeholder="Upload password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      />
      <input
        type="file"
        aria-label="CSV file"
        accept=".csv"
        onChange={(e) => setFile(e.target.files?.[0] || null)}
        className="rounded-lg border border-input-border bg-input px-3 py-2 text-sm"
      />
      <button
        onClick={submit}
        disabled={!file || !password || isUploading}
        className="rounded-lg bg-button px-3 py-2 text-sm font-medium text-button-text transition hover:bg-button-hover disabled:cursor-not-allowed disabled:bg-button-disabled"
      >
        {isUploading ? "Uploading..." : "Upload"}
      </button>
      {status && <p className="text-sm text-muted">{status}</p>}
    </section>
  );
}
