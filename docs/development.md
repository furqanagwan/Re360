# Developing ReXGlue with humans and coding agents

The SDK targets Windows x64, April 2026 PC GDK 260404 and D3D12. Read
[AGENTS](../AGENTS.md), the assigned issue and applicable ADRs before editing.
The same acceptance rules apply to a person and a coding agent. No previous
chat is required: behavior, source pins, decisions and results belong in the
repository or linked issue, not only in a conversation.

## Find the owner and the boundary

| Change | Owner/files | Primary check |
| --- | --- | --- |
| PPC generation, guest ABI | `src/codegen`, `include/rex/ppc` | Generated PPC tests, pinned corpus and regeneration |
| Runtime, memory, threads, guest objects | `src/system`, `src/kernel` | Process-isolated unit/guest-contract tests |
| Xenos, PM4, textures, render targets, shaders | `src/graphics`, `include/rex/graphics` | Synthetic GPU readback, affected shader paths and NVIDIA/title evidence |
| Native presentation and title host | `src/ui`, `include/rex/ui` | Host/UI tests and fullscreen/Guide integration |
| Xbox Guide/XUI implementation | `src/xbox` | XUI and Guide unit tests (`unit_tests [xui]`, `[guide]`) and the fuzzer |
| CLI, templates, installed SDK | `src/rexglue`, `resources`, `cmake` | Script/unit tests and an external installed consumer |
| 007 content/configuration | [007](https://github.com/furqanagwan/007) | Executable-bound configuration and private title validation |

CODEOWNERS routes this implementation fork's reviews to its maintainer. That
does not transfer third-party authorship or replace source notices. Shared
fixes belong in the SDK; titles select named profiles/defaults. No global
title-ID branches and no SDK dependency on the research emulator.

## C++ rules that matter here

Keep C++23, the supported Clang toolchain, `.clang-format` and `.editorconfig`.
Use the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
as design guidance, with these explicit guest-runtime boundaries:

- Own host resources with RAII. Use exclusive ownership unless sharing is
  required; make borrowed pointers/references and their lifetimes clear. COM
  resources use their established wrappers. Guest handle tables/refcounts and
  mapped guest memory have their own ownership contracts: a raw guest pointer
  is not automatically a host allocation to put in `unique_ptr`.
- Use spans/views for borrowed ranges and explicit fixed-width integers for
  guest data. Check bounds and arithmetic before indexing, copying or
  allocating. Preserve endian conversion, guest pointer width, packing,
  alignment and typed export argument order/status codes. Do not replace
  packed guest structures with host layouts.
- Keep lock ownership, callback lifetime, cancellation and teardown visible.
  Snapshot before calling external code when the contract allows it; never
  assume a recursive lock makes arbitrary callback re-entry safe. Test actual
  failure/teardown sequences rather than only successful construction.
- Keep public headers self-contained and interfaces narrow. Host Windows/GDK
  APIs belong behind host adapters rather than leaking into guest signatures.
  A plugin/DLL boundary requires the documented handshake/toolchain ABI checks;
  C++ source compatibility is not a stable binary ABI promise.
- Name numeric guest/hardware constants when it helps explain the contract.
  Preserve upstream names, notices and provenance where needed for comparison.
  Do not rename entire imported subsystems or mechanically rewrite SIMD,
  pointer arithmetic or strict floating-point code to satisfy style checks.
- Use target usage requirements for new CMake dependencies. Link/include a
  dependency where it is needed; avoid expanding directory/global flags.
  Existing strict FP and guest aliasing options are compatibility decisions,
  not candidates for an untested cleanup.

References: [CMake usage requirements](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html)
and [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/).

## A bounded change

Describe the trigger and expected behavior in the issue. Identify the files,
dependencies, relevant upstream changes/regressions, acceptance checks and
rollback before a major port. Capture the last-good baseline and source SHA.
Use a focused branch and commit one independently reviewable behavior at a
time. Avoid combining a port with unrelated backend removal or a large rename.

For an architecture change, write a superseding ADR. An ADR states direction;
release evidence states what was actually tested. Do not silently update
historical measurements, roadmap publication hashes or old issue bodies to
make current support look broader.

## Build, tooling and evidence

The canonical configure/build/test commands are in [AGENTS](../AGENTS.md).
Presets emit `compile_commands.json`; `.clangd` points to the GDK build tree.
Use the compile database for editor navigation and focused Clang-Tidy checks.
`.clang-tidy` exists, but a repository-wide clean static-analysis baseline is
not established. Do not claim it passes or auto-fix all legacy findings.

Typed kernel-export entry functions retain the guest-facing PascalCase name
with the `_entry` suffix used by their export registration. Clang-Tidy permits
that specific spelling; ordinary functions still follow PascalCase.

Pull-request lint runs Clang-Tidy on added or modified lines in compiled C++
files, with findings treated as errors. The runner uses native absolute paths
so its line filters match Windows diagnostics. Headers without their own
compilation database entry are skipped. To run the same check locally from an
x64 Visual Studio developer shell, configure without precompiled headers and
pipe the diff into the runner:

```powershell
cmake --preset win-amd64-gdk -B out/build/tidy -DREXGLUE_BUILD_TESTS=ON -DCMAKE_DISABLE_PRECOMPILE_HEADERS=ON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
git diff -U0 --no-color origin/main -- src include tests | python scripts/clang_tidy_changed.py --build out/build/tidy
```

CTest is the supported unit invocation: cases run in separate processes.
The shared test memory fixture owns one live guest address space. Mixing that
fixture with independent live `Memory` instances in one test process violates
the singleton contract; the combined unit executable is not an alternative
passing gate. Test isolation must be explicit, not a way to conceal failures.

A change to a file parser (XEX, XDBF, ISO/XDVDFS, STFS, XUI) also runs its
libFuzzer target with AddressSanitizer; see [fuzzing](fuzzing.md).

Hosted GDK software CI builds both configurations and runs unit/PPC cases.
Installed-runtime cases have a separate label and remain part of local CTest;
hosted software runs exclude them explicitly. GPU, actual package deployment,
controller, visual, save/load and title tests need their own evidence.

Run affected checks after each fix. Batch the expensive 007 generation/build
cycle after a selected SDK set; regenerate when codegen changes. Routine title
checks use one fullscreen candidate instance and disposable save copies.
Use a paired baseline/candidate run when a specific regression needs it. Keep
diagnostics bounded, extract result/hash summaries, then remove disposable raw
logs. Never commit game/console assets, generated game code, saves, private
captures or restricted SDK material.

Record commands, configurations, toolchain/source pins, discovered counts,
passes/skips/failures and blockers. Zero tests is an error. A passing build is
not compatibility, and unvisited title paths remain untested. End with a clean,
reviewable diff and an issue/PR description a new developer can understand.
