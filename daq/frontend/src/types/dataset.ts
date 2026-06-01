export type Dataset = {
  slug: string;
  title: string;
  filename: string;
  uploaded_at: string;
  size_bytes: number;
};

export type SchemaColumn = {
  name: string;
  type: string;
  unit?: string | null;
  display_name?: string;
  sample_values: string[];
};
