import { afterEach, describe, expect, it, vi } from "vitest";
import { apiFetch } from "./client";

describe("apiFetch", () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it("uses FastAPI detail strings as error messages", async () => {
    vi.stubGlobal(
      "fetch",
      vi.fn().mockResolvedValue(new Response(JSON.stringify({ detail: "Unknown dataset columns: Speed" }), { status: 400 })),
    );

    await expect(apiFetch("/api/datasets/example/chart-data")).rejects.toThrow("Unknown dataset columns: Speed");
  });

  it("uses validation messages from FastAPI detail arrays", async () => {
    vi.stubGlobal(
      "fetch",
      vi.fn().mockResolvedValue(new Response(JSON.stringify({ detail: [{ msg: "Field required" }] }), { status: 422 })),
    );

    await expect(apiFetch("/api/datasets/example/chart-data")).rejects.toThrow("Field required");
  });
});
