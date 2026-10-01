# Rutgers Formula Racing

This repository consists of several different important scripts for the Rutgers Formula Racing team. Each folder is descriptively named and complicated folders have their own `README.md` with further instructions/information.

---

Certain folders exist as submodules because of their necessity to stand alone. Click the folder to be brought to the dedicated repository for that code.

## Formatting and linting

Install Node.js 22.13+, [uv](https://docs.astral.sh/uv/getting-started/installation/),
and Zig `0.17.0-dev.1257+67b05e521` (the version required by the precharge firmware),
then run from the repository root:

```sh
git submodule update --init daq-website
npm ci
uv sync --locked --project python
npm run format
npm run check
```

`npm ci` installs Husky hooks. Commits check staged files; pushes check files
changed between the remote and pushed commits (all files for a new remote branch).
CI and `npm run check` check the whole repository. The checks include:
Prettier and ESLint for JavaScript and repository documents, Ruff formatting and
linting plus ty type checking for Python, clang-format and Cppcheck for C,
and `zig fmt`, `zig ast-check`, and host tests for Zig.
The hooks check formatting without changing or staging files; use `npm run format`
to apply formatting and safe lint fixes before committing.

GitHub Actions runs on pushes to every branch and on pull requests. Configure
repository rulesets to require the `Code quality / quality` status check to block
merges when checks fail; a workflow reports failed pushes but cannot prevent them.

Checks cover tracked and new, non-ignored source owned by this repository.
The `daq-website`, `temp-board-rewrite`, and `vectorNav` submodules need their own
pipelines in their repositories. Upstream STM32 drivers, copied HAL/device headers,
and generated system startup code are excluded. CubeMX files containing user code
are checked. Arduino sketches receive clang-format checks; board-specific Arduino
compilation is outside this pipeline.
