import { render, screen } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { beforeEach, describe, expect, it, vi } from "vitest";
import { uploadDataset } from "../api/datasets";
import { UploadPanel } from "./UploadPanel";

vi.mock("../api/datasets", () => ({
  uploadDataset: vi.fn(),
}));

describe("UploadPanel", () => {
  beforeEach(() => {
    vi.mocked(uploadDataset).mockReset();
  });

  it("shows upload failures", async () => {
    const user = userEvent.setup();
    vi.mocked(uploadDataset).mockRejectedValue(new Error("Invalid upload password"));
    render(<UploadPanel onUploaded={vi.fn()} />);

    await user.type(screen.getByPlaceholderText("Upload password"), "bad-password");
    await user.upload(screen.getByLabelText(/csv/i), new File(["a,b\n1,2\n"], "data.csv", { type: "text/csv" }));
    await user.click(screen.getByRole("button", { name: "Upload" }));

    expect(await screen.findByText("Invalid upload password")).toBeInTheDocument();
  });
});
