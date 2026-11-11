# Extraction from ReXGlue

Date: 2026-10-06. Source commit: `d1a87b4ef0a09c7a7813ab2a2b27976de01de203`.

Guide and XUI sources, public headers and their existing unit tests are now
owned by this repository. Original paths and notices are retained. Git history
is available at the immutable source commit; docs/source-history.txt records
the affected commits. No upstream subsystem was imported.

ReXGlue consumes a pinned submodule and attaches the sources to its existing
rexui object library. That preserves the single runtime copy of cvar and
logging state, DLL exports, installed headers and guest service integration.
The SDK keeps its CLI registration and title embedding helper as adapters.
ResolveFile was split from runtime.cpp into resolve_file.cpp so the reusable
scene runtime can link without the SDK's system-update loader. Its behavior
is unchanged. Standalone and SDK consumers are separate build paths; a title
using the SDK must not also link a second copy of xbox::ui_core.

The complete Guide still depends on ReXGlue input, achievements, content,
settings, UI dialogs and audio. The standalone scene layer is reusable now;
porting all Guide pages to another framework needs service adapters and new
validation. This extraction does not claim that work is complete.

## Issue ownership

Transfer source issues 127 (Guide epic), 132 (look/mods/cheats), 143 (Home and
blades), 145 (Manage Game), 153 (Guide title updates), 161 (keyboard), and
166 (PC backward-compatibility Guide) using GitHub transfer. Existing state,
comments and redirects are retained. The epic remains open.

Keep 137 (render resolution), 139 (input backends and battery), 141 (guest
achievement enumerators), 154 (disc/ISO loading) and runtime/codegen issues
in ReXGlue. Their Guide connections do not move ownership of the underlying
SDK service. Issue 153 also includes SDK codegen: future changes to generating
the update executables belong in ReXGlue, with a link to its transferred record.

The completed transfer map is below and in [machine-readable form](issue-transfer.json).

## Validation at extraction

Windows x64, Clang 22.1.8 and CMake 4.4.3. Standalone Debug and Release each
discover and pass one CTest test containing three Catch2 cases and 16 assertions.
Dependencies use the exact fmt/ImGui/Catch2 revisions pinned by the source SDK;
the local runs use those source checkouts through FetchContent source overrides.
No SDK runtime or private assets are linked to the standalone test.

The SDK's current-source Release Guide baseline and extracted-source suite
both pass 51 cases and 583 assertions, with six private-asset cases skipped
(57 cases discovered). The title application source compilation target also
builds. Broader SDK validation is recorded in its extraction document and
release evidence. The added Windows CI workflow tests dependency fetching on
a fresh runner. [CI at 62267d4](https://github.com/furqanagwan/xbox/actions/runs/37526640817)
passed on a fresh Windows runner, fetching dependencies and building/testing
both configurations without an SDK checkout.

Original issue labels were restored and verified after transfer. GitHub drops
custom labels when their definitions do not exist in the destination; the
missing definitions were copied from the SDK before restoring the labels.

Real title/pad interaction has not been repeated for this extraction. The open
epic retains that gate; no compatibility claims for another SDK are made.


| Original SDK issue | Guide issue | State | Comments preserved |
| --- | --- | --- | --- |
| [#127](https://github.com/furqanagwan/rexglue-sdk/issues/127) | [#1](https://github.com/furqanagwan/xbox/issues/1) | open | 0 |
| [#132](https://github.com/furqanagwan/rexglue-sdk/issues/132) | [#2](https://github.com/furqanagwan/xbox/issues/2) | closed | 0 |
| [#143](https://github.com/furqanagwan/rexglue-sdk/issues/143) | [#3](https://github.com/furqanagwan/xbox/issues/3) | closed | 0 |
| [#145](https://github.com/furqanagwan/rexglue-sdk/issues/145) | [#4](https://github.com/furqanagwan/xbox/issues/4) | closed | 0 |
| [#153](https://github.com/furqanagwan/rexglue-sdk/issues/153) | [#5](https://github.com/furqanagwan/xbox/issues/5) | closed | 1 |
| [#161](https://github.com/furqanagwan/rexglue-sdk/issues/161) | [#6](https://github.com/furqanagwan/xbox/issues/6) | closed | 0 |
| [#166](https://github.com/furqanagwan/rexglue-sdk/issues/166) | [#7](https://github.com/furqanagwan/xbox/issues/7) | closed | 4 |
