import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { existsSync, readFileSync } from "node:fs";
import { extname } from "node:path";

// Submodules and upstream STM32 drivers are maintained outside this repository.
const excluded =
  /^(daq-website|temp-board-rewrite|vectorNav)\/|\/Drivers\/|\/Core\/Inc\/stm32f(?:103.*|1xx_hal(?:_can|_flash)?)\.h$|\/Core\/Src\/system_stm32f1xx\.c$/;
const owned = (path) => !excluded.test(path) && existsSync(path);
const run = (command, args, options = {}) => {
  console.log(`\n> ${command} ${args.join(" ")}`);
  execFileSync(command, args, { stdio: "inherit", ...options });
};
const pythonTool = (tool, args) =>
  run("uv", ["run", "--locked", "--project", "python", tool, ...args]);
const pushRanges = (input) =>
  input
    .trim()
    .split("\n")
    .filter(Boolean)
    .flatMap((line) => {
      const [, local, , remote] = line.split(/\s+/);
      if (/^0+$/.test(local)) return [];
      return [
        [
          /^0+$/.test(remote)
            ? "4b825dc642cb6eb9a060e54bf8d69288fbee4904"
            : remote,
          local,
        ],
      ];
    });

const mode = process.argv[2];
if (mode === "test") {
  assert(excluded.test("vectorNav/python/example.py"));
  assert(excluded.test("daq-website/frontend/src/App.tsx"));
  assert(excluded.test("precharge-c/Drivers/hal.c"));
  assert(excluded.test("precharge-c/Core/Inc/stm32f1xx_hal_can.h"));
  assert(!excluded.test("precharge-c/Core/Inc/stm32f1xx_hal_conf.h"));
  assert(!excluded.test("precharge-c/Core/Src/main.c"));
  assert(!excluded.test("new-project/main.c"));
  assert.deepEqual(pushRanges("refs/heads/main abc refs/heads/main def\n"), [
    ["def", "abc"],
  ]);
  assert.deepEqual(pushRanges("refs/heads/main 000 refs/heads/main def\n"), []);
  assert.deepEqual(pushRanges("refs/heads/new abc refs/heads/new 000\n"), [
    ["4b825dc642cb6eb9a060e54bf8d69288fbee4904", "abc"],
  ]);
  assert.deepEqual(pushRanges(""), []);
  console.log("Quality scope checks passed.");
} else {
  assert(["check", "format"].includes(mode), "Use check, format, or test");
  const format = mode === "format" || process.argv.includes("--fix");
  const scope = process.argv[3];
  assert(
    !scope || ["--staged", "--push"].includes(scope),
    "Use --staged or --push",
  );
  const diff = ["diff", "--name-only", "-z", "--diff-filter=ACMR"];
  const queries =
    scope === "--staged"
      ? [[...diff, "--cached"]]
      : scope === "--push"
        ? pushRanges(readFileSync(0, "utf8")).map((range) => [
            ...diff,
            ...range,
          ])
        : [["ls-files", "-z", "--cached", "--others", "--exclude-standard"]];
  const files = [
    ...new Set(
      queries.flatMap((args) =>
        execFileSync("git", args, { encoding: "utf8" }).split("\0"),
      ),
    ),
  ].filter(owned);
  const originals =
    format && scope ? files.map((file) => readFileSync(file)) : [];
  const select = (extensions) =>
    files.filter((file) => extensions.includes(extname(file)));
  const web = select([
    ".js",
    ".mjs",
    ".cjs",
    ".json",
    ".yaml",
    ".yml",
    ".md",
    ".css",
    ".html",
  ]);
  const js = select([".js", ".mjs", ".cjs"]);
  const py = select([".py"]);
  const c = select([".c", ".h", ".ino"]);
  const zig = select([".zig", ".zon"]);

  if (web.length)
    run("npm", [
      "exec",
      "--",
      "prettier",
      format ? "--write" : "--check",
      ...web,
    ]);
  if (js.length)
    run("npm", [
      "exec",
      "--",
      "eslint",
      "--max-warnings=0",
      ...(format ? ["--fix"] : []),
      ...js,
    ]);
  if (py.length) {
    pythonTool("ruff", ["check", ...(format ? ["--fix"] : []), ...py]);
    pythonTool("ruff", ["format", ...(format ? [] : ["--check"]), ...py]);
    if (mode === "check")
      pythonTool("ty", [
        "check",
        "--project",
        "python",
        "--error-on-warning",
        ...py,
      ]);
  }
  if (c.length)
    pythonTool("clang-format", [
      format ? "-i" : "--dry-run",
      ...(format ? [] : ["--Werror"]),
      ...c,
    ]);
  if (mode === "check") {
    for (const project of new Set(
      c
        .filter((file) => file.endsWith(".c"))
        .map((file) => (file.includes("/") ? file.split("/")[0] : ".")),
    )) {
      const sources = c.filter(
        (file) =>
          (project === "."
            ? !file.includes("/")
            : file.startsWith(`${project}/`)) && file.endsWith(".c"),
      );
      if (sources.length)
        pythonTool("cppcheck", [
          "--enable=warning,performance,portability",
          "--check-level=exhaustive",
          "--error-exitcode=1",
          "--inline-suppr",
          "--std=c11",
          ...(["precharge-c", "rfr26-tempSensor"].includes(project)
            ? [
                "--platform=unix32",
                "-D__GNUC__",
                "-DUSE_HAL_DRIVER",
                "-DSTM32F103xB",
                `-I${project}/Core/Inc`,
                `-I${project}/Drivers/STM32F1xx_HAL_Driver/Inc`,
                `-I${project}/Drivers/CMSIS/Include`,
                `-I${project}/Drivers/CMSIS/Device/ST/STM32F1xx/Include`,
              ]
            : []),
          ...sources,
        ]);
    }
  }
  if (zig.length) {
    run("zig", ["fmt", ...(format ? [] : ["--check"]), ...zig]);
    if (mode === "check") {
      for (const file of zig.filter((file) => file.endsWith(".zig")))
        run("zig", ["ast-check", file]);
      run("zig", ["test", "precharge/src/test.zig"]);
    }
  }
  assert(
    originals.every((content, i) => content.equals(readFileSync(files[i]))),
    "Auto-fixes applied. Stage and commit the fixed files, then retry.",
  );
}
