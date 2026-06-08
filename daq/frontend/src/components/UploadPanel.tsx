import { useState } from "react";
import { Upload } from "lucide-react";
import { uploadDataset } from "../api/datasets";
import { Alert, Button, FieldInput, Panel } from "./ui";

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
    <Panel className="grid content-start gap-3 p-4">
      <div>
        <h3 className="text-base font-semibold">Upload CSV</h3>
        <p className="text-sm text-muted">Add a dataset for plotting.</p>
      </div>
      <FieldInput
        type="password"
        placeholder="Upload password"
        value={password}
        onChange={(e) => setPassword(e.target.value)}
      />
      <FieldInput
        type="file"
        aria-label="CSV file"
        accept=".csv"
        onChange={(e) => setFile(e.target.files?.[0] || null)}
        className="h-auto py-1.5 file:mr-3 file:rounded-md file:border-0 file:bg-surface-soft file:px-2.5 file:py-1 file:text-xs file:font-medium file:text-text"
      />
      <Button
        onClick={submit}
        disabled={!file || !password || isUploading}
        variant="primary"
      >
        <Upload size={15} aria-hidden="true" />
        {isUploading ? "Uploading..." : "Upload"}
      </Button>
      {status && <Alert tone={status.toLowerCase().startsWith("uploaded") ? "success" : "danger"}>{status}</Alert>}
    </Panel>
  );
}
