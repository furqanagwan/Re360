# ADR-019: XboxCPU, XboxGPU and XboxUI are the names people see

Date: 2026-11-12. Status: accepted owner direction. Implemented: README,
AGENTS.md, the component documentation, user-visible log, build and UI text,
and the 007 release notes. Supersedes AGENTS.md's rule that only `rexglue` and
`rex::` names are used, for prose; it keeps that rule for identifiers.

## Context

ADR-018 combined three repositories into Re360. Their old names stayed in
everything people read: ReXGlue (also called ReCPU in planning) for the
recompiler and runtime, ReGPU for the lean renderer, and "the Xbox guide" for
the Guide and its XUI runtime. Readers met three project names that no longer
match a repository, and log lines that named a renderer differently from the
documentation.

## Decision

1. **Three component names.** People see:
   - **XboxCPU**: the recompiler and the console runtime (from ReXGlue).
   - **XboxGPU**: the `regpu` renderer (from ReGPU).
   - **XboxUI**: the Xbox 360 Guide and the XUI runtime that plays it
     (`src/xbox`).

   The SDK as a whole stays **Re360**: build messages about the whole SDK
   (requirements, configuration summary, developer shells) say Re360.
2. **Only what people read changes.** Documentation, READMEs, UI text, log
   messages, CLI banners and release notes. Code does not: identifiers,
   namespaces (`rex::`, `regpu::`), file and folder names, CMake packages and
   options (`rexglue`, `REXGLUE_*`, `REGPU_*`), DLL and CLI names (`rexgpu-regpu`,
   `rexglue`), cvar names and values (`gpu_plugin=regpu`), window classes and
   on-disk folders (`%LOCALAPPDATA%\ReXGlue`) all keep their names, so builds,
   saves, settings and scripts keep working.
3. **Credits and history keep the upstream names.** Credits name ReXGlue and
   Scott's ReGPU with their links; upstream tracking, provenance and audit
   documents, and earlier ADRs describe those projects by their names. A first
   mention of a new name may say where it comes from ("XboxGPU, from ReGPU").
4. **The console's own names stay.** "Xbox Guide" where it means the console's
   menu as the console shows it (photographs of the 17559 dashboard, the
   console's scenes), and "Original Xbox Guide" for backward-compatibility
   titles.
5. **"Xbox" in these names** is the owner's choice, made knowing it is
   Microsoft's trademark; the README's notice that Re360 is not affiliated with
   or endorsed by Microsoft or Xbox stands.

## Consequences

- Logs say `XboxUI: ...` where they said `Xbox guide: ...`, and the regpu plugin
  logs `XboxGPU started at ...`; anything matching the old text must follow.
- `rexglue --version` prints "XboxCPU v... - Xbox 360 recompiler and runtime
  (Re360)"; the command is still `rexglue`.
- New documentation uses the three names; older documents are changed when
  they are next edited, not all at once.
