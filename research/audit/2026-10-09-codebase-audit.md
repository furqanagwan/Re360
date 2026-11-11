# Codebase audit, 2026-10-09

Goal: anyone, human or agent, can find their way around these repositories
and change them safely. The standard is clear names, no comments,
modern C++, small files, and knowledge kept in `research/`.

Scope: `rexglue-sdk` (with its thirdparty/xbox (a submodule until 2026-11-11) submodule, then named `xbox-guide`), `xbox`
and `007`. The `xenia-edge` fork is a reference and stays as upstream has it.

## Summary

The code already builds as C++23 and every file name is snake_case. The
problems are comments, a few very large files, mixed naming conventions
inherited from Xenia, and unnamed fields (`unk_*`). None of it needs a rewrite;
it needs steady, behavior-preserving cleanup in small pull requests.

| Measure | rexglue-sdk | xbox-guide |
| --- | --- | --- |
| Hand-written C++ files (excluding generated shaders) | 743 | 56 |
| Lines | 218,889 | 13,120 |
| Comment lines | 35,968 (16.4%) | 1,150 (8.8%) |
| C++ standard | C++23 | C++23 |
| File names | all snake_case | all snake_case |

How these were measured is under [Method](#method).

## Findings

### 1. Comments (all repos)

16% of SDK lines are comments. Most fall into three kinds:

- **Restating code** (`// Copy the buffer`): delete; rename if the code is unclear.
- **Hardware and history knowledge** (EDRAM layout, why a register reads
  `0x0000200E`, which title needed a workaround): move to `research/gpu/`,
  `research/cpu/` and so on, then delete from code.
- **License and attribution headers**: keep. They are legally required.

Heaviest areas: `include/rex/codegen` (35%), `include/rex/ui` (32%),
`include/rex/graphics` (30%), `src/graphics/util` (29%).

### 2. Very large files (rexglue-sdk)

| Lines | File |
| --- | --- |
| 6,000 | `src/graphics/d3d12/command_processor.cpp` |
| 5,706 | `src/graphics/d3d12/render_target_cache.cpp` |
| 4,512 | `src/graphics/pipeline/shader/spirv_translator.cpp` |
| 4,245 | `src/graphics/d3d12/pipeline_cache.cpp` |
| 3,833 | `src/graphics/pipeline/shader/spirv_translator_rb.cpp` |
| 3,446 | `src/graphics/pipeline/shader/dxbc_translator.cpp` |
| 2,314 | `src/system/xmemory.cpp` |
| 2,113 | `src/codegen/function_scanner.cpp` |
| 1,558 | `xbox-guide: src/ui/guide/xbox_guide.cpp` |

Split by responsibility (for example the command processor's draw setup,
resolve and memexport paths) when the area is next changed. Large GPU splits
should wait for issue #53's DXIL work so they don't collide.

### 3. Naming

- **Two function styles.** Most code is PascalCase (Google style), but the
  codegen builders are 415 snake_case free functions (`build_fres`). Pick
  PascalCase everywhere (`BuildFres`) in one mechanical PR.
- **Xenia's `x` prefix.** 103 files such as `xmemory.cpp`, `xthread.cpp`
  and `xam_*`. `xam` and `xboxkrnl` are real Xbox module names and stay.
  Kernel object files (`xthread`, `xevent`, `xmutant`) would read better as
  `kernel_thread` and so on.
- **Unknown fields.** 355 `unk*` names (`unk_04`, `unk1`). Rename each one when
  its meaning is found, and record the evidence in `research/`.
- **Magic register numbers.** Some code switches on raw register indexes
  with a comment naming the register. Use the generated `XE_GPU_REG_*` names
  (done for the registers touched in PR #224).

### 4. Leftovers

- **Stubbed kernel exports.** 2,402 `REX_EXPORT_STUB` lines are listed
  exports that do nothing. That is expected for a console kernel, but they
  belong in one generated table instead of being spread through source files.
- **Old platform code.** 8 Linux/macOS/POSIX conditionals remain after those
  platforms were retired; remove them.
- **TODO markers.** 759 TODO/FIXME/HACK markers. Each should become an issue
  or be deleted.
- **SPIR-V translator.** 11 files are kept for the DXIL path (#53). Keep
  them, and label that path clearly as `dxil` once #53 settles.

### 5. Repository hygiene

- `main` is now protected in all three repos, with PRs, CI and review required.
- PR template: what and why, how it was tested, before/after screenshots
  for UI changes, standards checklist.
- `docs/` mixes how-tos, decisions, evidence logs and dated audits. Keep
  how-tos and decisions in `docs/`, move evidence and investigations into
  `research/` over time, and drop dated snapshots once superseded.
- `007` has no CI. Add a light check (Markdown links, `.toml` syntax) so the
  protected branch has something to require.

## Plan

Each step is its own issue and PR, and each PR must keep behavior identical
(the same tests pass, before and after).

1. Remove the 8 POSIX/Linux/macOS leftovers.
2. Rename the codegen builders to PascalCase.
3. Turn TODO markers into issues or delete them.
4. Comment migration, one area per PR:
   `codegen` → `ppc` → `kernel` → `system` → `audio` → `input` → `filesystem` →
   `ui` → `graphics` (graphics last, after #53). Each PR moves knowledge
   into `research/<area>/` and deletes the rest.
5. Split the largest files when their area is next changed.
6. Rename kernel object files (`xthread` → `kernel_thread` and so on).
7. Rename `unk*` fields as their meaning is found.
8. Add a `clang-tidy` naming check in CI so new code follows the rules.

## Method

`audit_metrics.py` walked `src`, `include` and `tests/unit` (skipping
`bytecode` and `generated` folders). It counted lines starting with `//`,
`/*` or `*` as comments, and checked file names against `[a-z0-9_]+`.
Other counts came from `grep` over `src` and `include`: `REX_EXPORT_STUB`,
`TODO|FIXME|HACK|XXX`, `REX_PLATFORM_(LINUX|MAC|POSIX|ANDROID)`, `\bunk\w*`.
