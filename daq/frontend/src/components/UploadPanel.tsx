import { useState } from "react";
import { LockKeyhole, Upload } from "lucide-react";
import { uploadDataset } from "../api/datasets";
import { Alert, Button, FieldInput, Label, Panel } from "./ui";

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
    <Panel className="grid content-start gap-4 overflow-hidden">
      <div className="border-b border-border px-4 py-3">
        <div className="flex items-center gap-3">
          <span className="flex h-9 w-9 items-center justify-center rounded-md border border-border bg-surface-soft text-button">
            <Upload size={17} aria-hidden="true" />
          </span>
          <div>
            <h3 className="text-base font-semibold">Upload CSV</h3>
            <p className="text-sm text-muted">Add a dataset for plotting.</p>
          </div>
        </div>
      </div>
      <div className="grid gap-3 px-4 pb-4">
        <Label className="grid gap-1.5">
          Upload Password
          <span className="relative">
            <LockKeyhole size={14} aria-hidden="true" className="pointer-events-none absolute left-3 top-1/2 -translate-y-1/2 text-subtle" />
            <FieldInput
              type="password"
              placeholder="Upload password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              className="pl-9"
            />
          </span>
        </Label>
        <Label className="grid gap-1.5">
          CSV File
          <FieldInput
            type="file"
            aria-label="CSV file"
            accept=".csv"
            onChange={(e) => setFile(e.target.files?.[0] || null)}
            className="h-auto py-1.5 file:mr-3 file:rounded-md file:border-0 file:bg-surface-soft file:px-2.5 file:py-1 file:text-xs file:font-medium file:text-text"
          />
        </Label>
        <Button
          onClick={submit}
          disabled={!file || !password || isUploading}
          variant="primary"
          className="w-full"
        >
          <Upload size={15} aria-hidden="true" />
          {isUploading ? "Uploading..." : "Upload"}
        </Button>
        {status && <Alert tone={status.toLowerCase().startsWith("uploaded") ? "success" : "danger"}>{status}</Alert>}
      </div>
    </Panel>
  );
}
