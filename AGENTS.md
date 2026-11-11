# Canonical development instructions

Read `README.md`, `docs/investigation.md`, relevant ADRs and the assigned issue.
This file is the handoff for coding agents and humans;
no previous chat context is required.

## Mission and non-negotiable architecture

Re360 (`furqanagwan/Re360`) is a Windows PC / April 2026 GDK / D3D12-only static
Xbox 360 recompilation SDK. It combines the ReXGlue SDK (the recompiler and
runtime, also called ReCPU) with ReGPU (a second Xenos renderer); see ADR-018.
Identifiers keep the names `rexglue` and `rex::`. GDK is required, not optional
(ADR-016); GDK-free builds are retired. Project Helix is a future validation
target, not a hardware or deployment support claim. Current legacy platforms/backends
remain only until the documented replacement and regression gates are met.
`furqanagwan/xenia-edge` is a controlled reference/experiment fork, never a runtime
dependency. Upstream Xenia, Canary, Edge and ReXGlue are read-only references.

Do not reintroduce Vulkan, Linux or macOS as supported targets; restore Xenia's
PPC JIT; copy upstream subsystems blindly; make game-specific hacks global;
remove compatibility behavior just to compile; ignore upstream regression
history; claim untested compatibility; or close issues without required tests.
Do not delete current legacy code before its removal issue's gates are met.
GPU shader translation is allowed and is distinct from CPU JIT. SPIR-V IR use
requires a shader architecture decision and does not imply a Vulkan backend.

## Repository map

* `src/codegen`, `include/rex/codegen`, `include/rex/ppc`: static generation/ABI.
* `src/system`: runtime, function dispatcher, XEX, memory, guest objects/threads.
* `src/kernel`: XboxKrnl/XAM typed exports; preserve guest signatures and status.
* `src/graphics`, `include/rex/graphics`: Xenos/PM4, shaders, textures, EDRAM,
  D3D12 plugin (`rexgpu-xenos`, the default). `src/ui/d3d12` owns host
  device/presentation.
* `src/regpu`, `include/regpu`, `tools/regpu`: ReGPU, the `rexgpu-regpu` plugin
  (`gpu_plugin=regpu`). Read `docs/regpu.md` and `docs/regpu-debugging.md`
  before changing it. It keeps its own style and comments and stays
  game-neutral; `src/regpu/plugin` is SDK code and follows the rules below.
* `src/xbox`: the Xbox 360 interface: the XUI scene runtime (`src/xbox/ui/xui`)
  and the Guide (`src/xbox/ui/guide`), with their tests, compiled into
  rexruntime. Keep the public include paths (`<rex/ui/xui/...>`,
  `<rex/ui/guide/...>`) stable. See `docs/xbox-guide.md`. Never commit console
  updates or assets extracted from them.
* `src/audio`, `src/input`, `src/filesystem`, `src/core`: guest services/host adapters.
* `src/rexglue`, `resources`: CLI and generated project templates.
* `tests/unit`, `tests/ppc`: Catch2 unit and generated PPC instruction tests.
* `cmake`, `thirdparty`, `.github/workflows`: dependencies, exports and builds.
* `docs/roadmap`: exact GitHub issue bodies/index; `docs/upstream-tracking.md`:
  provenance; `docs/regression-strategy.md`: validation contract.

## Build and test

Use an x64 Visual Studio developer shell with LLVM Clang ≥18, CMake ≥3.25,
Ninja, Windows SDK and initialized submodules. `win-amd64` is a CPU architecture.

```powershell
git submodule update --init --recursive
cmake --preset win-amd64-gdk -DREXGLUE_BUILD_TESTS=ON
cmake --build --preset win-amd64-gdk-debug
ctest --preset win-amd64-gdk-debug --output-on-failure --no-tests=error
cmake --build --preset win-amd64-gdk-release
ctest --preset win-amd64-gdk-release --output-on-failure --no-tests=error
cmake --install out/build/win-amd64-gdk --config Release
```

PPC tests require bundled `tools/binutils/powerpc-none-elf-{as,ld,nm}.exe` and
their runtime DLLs. Confirm CTest discovers actual tests; zero tests is not a pass.
Precompiled DXBC shaders are built from `src/graphics/shaders` sources with
`python scripts/build_shaders.py [name ...]` (FXC from the Windows SDK); never
hand-edit `bytecode/` headers, and keep `--check` (CTest
`shaders.bytecode_reproducible`) passing.
The required `win-amd64-gdk` preset (`REXGLUE_USE_GDK`, pinned edition 260404)
is documented in `docs/gdk-toolchain.md`; it is proven on VS Community only, so
do not describe it as a Microsoft-supported pairing. `win-amd64` is a GDK alias;
do not create a separate GDK-free configuration. For planning/docs
changes run `python scripts/validate_roadmap.py`, `python scripts/check_docs.py`
(links and paths in README/AGENTS/CONTRIBUTING/docs) and `git diff --check`.
Support claims live in `docs/release-evidence.md`; update it with evidence
whenever a configuration's status changes.

## Conventions and Git workflow

Code has no comments apart from license headers: names say what code does
(ReGPU's renderer under `src/regpu` is the exception, per ADR-018).
Hardware behavior, workarounds, provenance and measurements go in
[research](research/README.md), one Markdown file per topic, updated in the
same pull request as the code. Use C++23, PascalCase functions and types,
snake_case files, and no `unk` names in new code.
Follow `.clang-format` (Google-derived, two spaces, 100 columns), `.editorconfig`
and [development guidance](docs/development.md), plus local namespace/typed-import
conventions. Keep guest endian, pointer width,
structure packing and return codes explicit. Prefer narrow adapters over leaking
host APIs into guest interfaces. Preserve notices and third-party licenses.

Check status and preserve user edits before work. Use a focused topic branch in
the implementation fork for implementation; never create upstream branches,
issues or PRs. Do not force-push or rewrite unrelated work. Keep migrations
bisectable and avoid mixing a code port with removal of unrelated backends.
Issue publication is permitted when the task requests it; no extra approval is
implied by this document. Otherwise follow the user's scope.

## Porting, regression and compatibility policy

Pin source SHA and PR head/merge state; read the diff, motivation, comments and
follow-up regressions. A closed PR may be rejected. Compare existing behavior
before calling something missing. Record source repo, commit, PR/issues, date,
title scope, classification A–H, known regressions, adaptation and tests in
`docs/upstream-tracking.md`. Unknown data stays unknown.

Classify correctness, compatibility, title-specific, driver-specific,
experimental and regression fixes separately. Do not import JIT safepoints,
thread teardown or exception machinery into static code without a separate
design. CPU byte patches require regeneration or explicit generated overrides.
Follow ADR-005 and ADR-009 for title profiles: behavior goes in the SDK fix
catalog and titles opt in by name; never branch on a title ID.

Establish a last-good baseline before major changes. Run relevant unit/PPC,
synthetic and representative-title tests plus AMD/NVIDIA/Intel gates for GPU
changes. Preserve saves; use disposable copies for failure injection. Record
missing hardware/test content as blocked. Do not distribute game/SDK material.
Update README when user-visible workflow/status changes, ADRs for architecture,
tracking ledger for ports and regression records for failures.

## Definition of done and agent-ready

An issue is done only when acceptance criteria, required tests, vendor/deployment
gates and docs are complete, limitations are recorded, and upstream provenance
and regression checks are linked. A build alone never proves compatibility.
`agent-ready` additionally requires settled architecture, completed dependencies,
reviewed relevant upstream items, identified files, bounded regression risk and
measurable tests. Remove that label if evidence or dependencies invalidate it.
The roadmap's initial ready queue was RG-GDK-001; it is a historical publication
record, not the current work assignment. Check live issues and the maturity audit
before choosing work. No migration is ready before its baseline/research gates.
Report precise blockers and leave work open.

Prioritize bounded SDK issues before rebuilding titles. Run affected software
checks after each fix; batch the expensive title rebuild/test cycle after the
selected SDK changes. Current representative titles are the three 007 games
only. Use fullscreen, one candidate run per routine check, quiet bounded
logging and disposable save copies. Paired runs remain appropriate for a
specific regression comparison. Keep small evidence summaries and remove
disposable verbose logs after extracting results; old raw logs were removed
at the owner's request on 2026-10-07. Audit/queue evidence is in
[the maturity audit](docs/sdk-maturity-audit-20261007.md).
