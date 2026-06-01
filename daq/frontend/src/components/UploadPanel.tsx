import { useState } from "react";
import { uploadDataset } from "../api/datasets";

type Props = {
  onUploaded: (slug: string) => void;
};

export function UploadPanel({ onUploaded }: Props) {
  const [password, setPassword] = useState("");
  const [file, setFile] = useState<File | null>(null);
  const [status, setStatus] = useState("");

  async function submit() {
    if (!file) return;
    try {
      const result = await uploadDataset(file, password);
      setStatus(`Uploaded ${result.filename}`);
      onUploaded(result.slug);
    } catch (error) {
      setStatus(error instanceof Error ? error.message : "Upload failed");
    }
  }

  return (
    <section className="panel">
      <h3>Upload CSV</h3>
      <input type="password" placeholder="Upload password" value={password} onChange={(e) => setPassword(e.target.value)} />
      <input type="file" accept=".csv" onChange={(e) => setFile(e.target.files?.[0] || null)} />
      <button onClick={submit} disabled={!file || !password}>Upload</button>
      {status && <p>{status}</p>}
    </section>
  );
}
