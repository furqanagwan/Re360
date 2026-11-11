# Release evidence

What this fork supports, the evidence behind each claim, and what is still open
(RG-GDK-025). A configuration is listed as **supported** only when a linked run
on a recorded revision backs it. **Opt-in** means it works in the recorded runs
but is not the default. **Blocked** means it has not been run and why. Nothing
here claims results on hardware, titles or machines that were not used.

Recorded on 2026-09-27 at `main` `11644e2` or later on one development machine:
- Windows 11 Home 10.0.26340.
- VS 2026 Community 18.10.2 (MSVC 19.51.36260 libraries), LLVM Clang 22.1.8,
  CMake 4.4.3, Ninja, Windows SDK 10.0.26100.0.
- PC GDK 260404 and Gaming Services 38.117.18001.0.
- NVIDIA GeForce RTX 5080 Laptop GPU with driver 32.0.16.1714.

## Configurations

| Configuration | Status | Evidence | Limitations |
| --- | --- | --- | --- |
| SDK build and tests, `win-amd64` Debug/Release | Builds; Release suite passes; Debug has one known fixture failure | Historical fresh checkout below; current [console source checks](#console-source-scenes-2026-10-07): Release 2,098 passed / 13 skipped; Debug baseline failure recorded below | One machine; the Debug tessellation assertion also reproduces before extraction; private Guide asset cases skip |
| SDK build and tests, `win-amd64-gdk` Debug/Release | Supported (opt-in preset) | `ctest` 1,839/1,839 at `6223f54`; [GDK toolchain](gdk-toolchain.md) | VS Community only (Microsoft names Professional/Enterprise); GPU fixtures run serially |
| SDK install and external consumer | Supported | `tests/gdk_consumer` against the installed prefix ([GDK toolchain](gdk-toolchain.md)); Quantum of Solace built from `out/install/win-amd64` and `out/install/win-amd64-gdk` | — |
| Title project (`rexglue init`, `codegen`, build) | Supported for the title run | Quantum of Solace codegen, build and link against both installs ([baseline capture](baseline-capture.md)) | One title; other XEXs may need title-specific analysis |
| D3D12 rendering, NVIDIA | Supported for the recorded scenes | GPU fixtures (RG-GDK-006 to 012); Quantum of Solace 90 s scene pass | Scene pass, not visual accuracy against hardware |
| ReGPU plugin (`gpu_plugin=regpu`) | Opt-in; one title run (Blood Stone) | 2026-11-10, Release, on the NVIDIA GPU above: `gpu.regpu_plugin_smoke` (PM4 stream, guest threads, register range, read-pointer write-back, and microcode-shaded draws at 1x and 4x MSAA resolved to guest memory with exact colours); `gpu.regpu_present` (a 1280x720 frame swapped into a host-owned window, presented picture read back exact; also at 1024x600 and 1152x640); `regpu_linkcheck` links ReGPU alone; the whole Release suite with both plugins: 2,357 run, 0 failed 2026-11-11: Blood Stone (disc 42DBE473, Release, fullscreen 3840x2160) on the same GPU: title, menus, first levels, the Xbox Guide over the game, and setup-time shader preparation (465 shaders, 755 pipelines, 12 s from a cold cache). Later on 2026-11-11, after the small-resolve write-back and D24FS8 depth-fill fixes: the Istanbul cistern mission, 3 minutes, 30 fps (the title's cap) over 4,261 frames, no log errors; before them the same scene was white. All 222 shaders in its cache also compile to signed DXIL with DXC (`-HV 2018`), which is not used at runtime yet. Fixtures cover constant-colour draws only | Not visually compared with a console or `xenos` frame by frame; the cistern still shows a yellow cast that may be the game's grading, unchecked against `xenos`; Quantum of Solace and Legends not run; see [ReGPU](regpu.md) |
| D3D12 rendering, Intel Graphics | Limited synthetic coverage | getBCF readback fixtures in Debug/Release, DXBC/strict DXIL (2026-10-08) | No Intel title or broader regression run |
| D3D12 rendering, AMD | Blocked | No AMD GPU available ([ADR-007](adr/ADR-007-local-gpu-validation-scope.md)); WARP runs are supplementary | Vendor-specific paths unverified |
| Input, window and audio: SDL3 | Removed (RG-GDK-033, 2026-09-28) | Old configs naming `sdl` start the native backend with a warning (`unit_tests [audio][xaudio2]`) | — |
| Input: XInput (default without the GDK) | Default | Quantum of Solace standard run with the ROG Raikiri (2026-09-28) | Subtypes from XInput; four pads not run |
| Input: GameInput (default in GDK builds) | Default | [GameInput](gameinput.md); Quantum of Solace native run after the PR #82 fix | No guide button; inbox runtime 0.2309 / redist 3.5.270 only |
| Window: Win32 (default) | Default | [Windowing](windowing.md); `gpu_tests [gpu][win32]`; Quantum of Solace native run | Multi-monitor DPI moves not run |
| Audio: XAudio2 (default) | Default | [Audio output](audio-output.md); `unit_tests [audio][xaudio2]`; Quantum of Solace native run | Physical device unplug not run; one HDMI endpoint |
| Gaming Runtime lifecycle (`gaming_runtime`) | Supported in GDK builds | [Gaming Runtime and PC packaging](gdk-packaging.md); Quantum of Solace native run logs "Gaming Runtime ready" | Missing or old Gaming Services tested with fakes only |
| `MicrosoftGame.config` and loose registration | Supported | `gdk.gameconfig_schema`; `gdk.smoke_registered` ([packaging](gdk-packaging.md)) | Registration as an application; elevated "as a game" not run |
| MSIXVC package, install, uninstall | Supported for the smoke title | `gdk.smoke_installed`; validator-clean Release package | Recompiled title not packaged; no Store submission |
| Saves and content | Supported in synthetic tests | `kernel_tests` ([content persistence](content-persistence.md)); saves survive package reinstall | Single profile; no power-loss test |
| PIX captures and host markers | Supported (NVIDIA) | PIX 2603.25; `gpu.pix_capture` (fixture capture, labeled events, debug-layer replay, no PIX imports) and `unit_tests [debug_markers]` ([PIX captures](baseline-capture.md#pix-for-windows-captures-rg-gdk-028)); Quantum of Solace frame 1800 captured with `d3d12_capture_frame`, labeled, debug-layer replay clean | Markers cover submissions, draws, resolves and swaps only; programmatic captures have no screenshot; AMD/Intel not run |
| ARM64 host | Blocked | No ARM64 MSVC libraries (RG-GDK-030/031) | — |
| DirectStorage | Not started | RG-GDK-029 | — |
| Fresh machine | Blocked | Only the development machine was available | The fresh-checkout run models a new clone, not a new PC |

## PC host experience (2026-10-07)

Revision: topic branch `pc-backcompat-experience`, based on `248d6c3`, Guide
submodule `3d5ea575a0f6a6ceb9f0c39c097d5c06c59b72aa`. The runs below test the
changes included in this commit; the logged environment now has VS Community
18.10.3, with the same Clang, Windows SDK, GDK and NVIDIA device recorded above.
See [coverage and remaining gates](pc-backward-compatibility.md).

- Standard Release: full build and CTest, 2,087 discovered, 2,076 passed,
  11 skipped, zero failures (170.89 s). Includes shader reproducibility and
  1,473 generated PPC cases. Logs: `out/bc-release-final-build.log` and
  `out/bc-release-final-ctest.log`.
- Standard Debug: full build and CTest, 2,087 discovered, 2,075 passed,
  11 skipped and the one baseline tessellation assertion (200.76 s). All 512
  unit cases pass or skip; the failure is in the GPU fixture. Logs:
  `out/bc-debug-final-build.log` and `out/bc-debug-final-ctest.log`.
- CTest inventories: Debug and Release each discover 2,087 tests; all 2,085
  SDK executable arguments per configuration, including nested PowerShell
  commands, match the requested configuration. This validates the
  [discovery fix](regression-strategy.md#ctest-configuration-discovery-2026-10-07).
  Logs: `out/bc-final-test-configuration-check.log` and corresponding
  `out/bc-*-final-discovery.json`.
- GDK Release: runtime/unit target build and the combined
  `[launch_settings],[language],[library_art],[cvar],[input],[gdk]` filter passed
  89 cases / 765 assertions. Log: `out/bc-gdk-final-targeted.log`. This is
  targeted evidence, not a new full GDK or service compatibility claim.
- Private reference Flash: three-tab backward-compatibility Guide and font
  conversion tests passed 2 cases / 28 assertions each in standard Debug,
  standard Release and GDK Release. The tests read the locally installed
  Fuzion Frenzy Flash directory; no assets were copied or redistributed.
  Logs: `out/bc-reference-guide-tests.log`,
  `out/bc-reference-guide-debug-tests.log`,
  `out/bc-reference-guide-gdk-tests.log`.
- Isolated Release install `out/bc-install`: the installed launcher consumer
  builds and links with the shipped ReXApp source and GPU plugin. Native art
  checks pass against the installed CMake helper for separate base/TU EXEs:
  RT_ICON, RT_GROUP_ICON, retained manifest, large/small Shell extraction,
  five PNG dimensions, collision protection and preserved config identity.
  Logs: `out/bc-final-consumer.log`, `out/bc-final-installed-art.log`.
  The isolated CMake user-package registry entry was removed afterwards.

The launcher unit cases draw ImGui without submitting GPU frames. A hidden
consumer window did not paint; interactive rendered launch/Play flow, physical
controller navigation/handoff, real-title settings and Xbox app registration
remain pending. Existing 007/NHL installations were not changed. Account,
cloud and service achievements require independent title/service registration
and failure/offline/account-switch validation. No whole-experience parity,
original Xbox execution or additional vendor support is claimed.

## Guide extraction (2026-10-06)

For the source/issue split on 2026-10-06, the
[Guide extraction record](xbox-guide-extraction.md#validation-2026-10-06)
records the component pin, matching before/after Guide suites, full standard
Debug/Release builds and CTest, GDK Release Guide checks, an isolated Release
install and installed consumer, and standalone Windows CI without an SDK.
The existing Debug tessellation assertion remains open in the
[regression record](regression-strategy.md#debug-tessellation-fixture-2026-10-06).
Private Guide assets and an owner pad/title session were unavailable; no
current title compatibility claim is added. GDK testing still uses VS Community
and does not establish Microsoft's supported toolchain pairing.

## Checked game sources (2026-10-07)

Revision: topic `pc-game-source`, based on `799a5ea`, with the Guide pin above.
[ADR-014](adr/ADR-014-game-source-and-media-recovery.md) records the source and
media recovery contract. Same development machine as the PC host checks.

- Standard Release full build/CTest: 2,109 discovered, 2,098 passed,
  11 skipped, zero failures (176.51 s), including 534 unit and 1,473 PPC cases.
  Logs: `out/source-release-final-build.log`,
  `out/source-release-final-ctest.log`.
- Debug targeted source/disc/codegen/launch UI: 36 cases / 499 assertions pass.
  Log: `out/source-debug-final-tests.log`.
- Standard Debug full build/CTest: 2,109 discovered, 2,097 passed, 11 skipped
  and the single pre-existing GPU tessellation fixture failure (212.15 s).
  All 534 unit cases pass or skip. Logs: `out/source-debug-final-build.log`,
  `out/source-debug-final-ctest.log`.
- GDK Release runtime/unit build and source/disc/codegen/launch/GDK filter:
  50 cases / 575 assertions pass. Logs: `out/source-gdk-final-build.log`,
  `out/source-gdk-final-tests.log`. This is targeted GDK evidence.
- Final review added entrypoint path validation and rechecked Release:
  36 cases / 511 assertions pass in both standard Debug and Release
  (`out/source-debug-reviewed-tests.log`, `out/source-release-reviewed-tests.log`).
  Final GDK Release review passes 50 cases / 587 assertions
  (`out/source-gdk-reviewed-tests.log`). The installed ReXApp consumer
  also compiles the entrypoint override check
  and the shared controller adapter (`out/source-consumer.log`).
- Isolated Release installation and external launcher consumer build/link
  pass. Native art regression passes against the installed CMake helper
  (`out/source-install.log`, `out/source-installed-art.log`). The isolated
  CMake package registry entry was removed; existing title installs are intact.

Synthetic tests cover sector-aligned reads, actual image truncation on open
handles, malformed directories, source identity, game:/d: mounting, atomic
extraction, cancellation, concurrent same-media recovery, UI-thread reads and
shutdown. Dialog tests draw ImGui without GPU frames; controller events are
injected. Physical pad/optical media, substituted-drive removal, original/TU
real-title launch and rendered interactive presentation remain unverified.
Console XuiMessageBox3, Active Downloads and friendly mismatch names remain
open. Issue #154 is not closed and whole-experience parity is not claimed.

## Source picker as a Guide page and mismatch names (2026-10-08)

The first-run picker is the in-game Guide's list page (xbox-guide
`GuideListPage`/`GuideFileBrowser`, pinned `1aeef33`). Owner check: Quantum of
Solace built against this SDK, launched without a source, browsed to its ISO
with a controller, verified it and launched (NVIDIA, fullscreen, disposable
user folder). Extraction, the wrong-game page and the disc-drive page were not
seen on screen.

A source holding another game names both. Codegen records the original XEX's
XDBF title (`source_title_name`); the check decodes the chosen XEX's image
(AES-CBC, basic or LZX compression, retail then development key) only when the
title IDs differ, with every offset bounded. Synthetic tests cover a named
image and six malformed variants; a local test decoded the retail Quantum of
Solace, Blood Stone and Legends executables (encrypted, LZX). Existing
generated titles keep the ID-only message until regenerated.

Debug CTest: 2,711 selections, two failures, both recorded before this
change: the tessellated-quad GPU fixture (regression strategy) and
`ppc_corpus.instr_mtmsrd`'s Debug abort (below). Title-update launches,
physical optical drives and media removal remain #154 gates.

## Console source scenes (2026-10-07)

Revision: topic `pc-guide-source-scenes`, based on `98f006b`; Guide
`db55a4d` ([component PR #8](https://github.com/furqanagwan/xbox/pull/8)).
The SDK supplies file picking/I/O, resources and immutable copy-history
snapshots. Guide-owned message-box and activity models remain usable through
the standalone scene target without SDK runtime dependencies.

- Standalone Guide Debug/Release build and CTest pass: one CTest entry runs
  five synthetic cases / 48 assertions. Logs:
  `out/message-box-core-{debug,release}-{build,tests}.log`.
- Standard Release final full build/CTest: 2,111 discovered, 2,098 passed,
  13 skipped, zero failures (179.00 s). Logs:
  `out/source-scenes-final-release-build.log`,
  `out/source-scenes-final-release-ctest.log`.
- Standard Debug final full build/CTest: 2,111 discovered, 2,097 passed,
  13 skipped and the one baseline GPU tessellation assertion (205.93 s).
  All 536 unit cases pass or skip. Logs:
  `out/source-scenes-final-debug-build.log`,
  `out/source-scenes-final-debug-ctest.log`.
- Final private/source/disc/launch/TU checks: standard Debug 41 cases /
  582 assertions, standard Release 41 cases / 583 assertions, GDK Release
  55 cases / 659 assertions, all passing. Logs:
  `out/source-scenes-private-{debug,release,gdk}-tests.log`.
  These use the locally installed Fuzion Frenzy Flash directory, including
  the three source choices, recovery default focus/controller A activation,
  native Disc/Retry selection and Active Downloads template. A synthetic ISO
  also exercises UI extraction, local-folder handoff and completed-copy history.
- Follow-up fallback default-focus check: the ImGui recovery fallback also
  starts on Leave Game. Source/disc/launch/TU/private filters pass 41 cases
  in Debug (590 assertions) and Release (591 assertions), with unchanged
  case counts. GDK Release passes 55 cases / 667 assertions. Logs:
  `out/source-scenes-fallback-{Debug,Release,gdk}-tests.log`.
  This narrow UI follow-up uses the full-suite baseline above; it does not
  change GPU/PPC behaviour.
- Isolated installed consumer compiles and links the updated ReXApp host
  adapter (`out/source-scenes-install.log`, `out/source-scenes-consumer.log`).
  Installed headers include the neutral scene APIs; the isolated CMake package
  registry entry is removed afterwards.

Two additional private cases account for the skipped-count increase from 11
to 13 in asset-free full CTest. Private checks pass separately. UI tests draw
ImGui without GPU frames and inject controller state; painted/physical pad
validation remains open. Images/optical media removal, friendly mismatch
names and real original/TU title launches remain #154 gates. Media dialogs
pair guest input blocking with XAM system-UI state/notifications and release
it on action or shutdown; that host lifecycle needs a real-title check.
No whole PC parity, service entitlement or original Xbox execution is claimed.

## Fresh checkout (2026-09-27)

In a new x64 developer shell (`vcvars64.bat`) the README's commands were run
verbatim against `https://github.com/furqanagwan/rexglue-sdk.git` at `11644e2`,
into an empty `C:\rg025fresh`:

| Step | Command | Result |
| --- | --- | --- |
| Clone | `git clone --recurse-submodules …` | OK (`11644e2`, all submodules at their pins) |
| Submodules | `git submodule update --init --recursive` | OK (nothing left to fetch) |
| Configure | `cmake --preset win-amd64 -DREXGLUE_USE_D3D12=ON -DREXGLUE_USE_VULKAN=OFF -DREXGLUE_BUILD_TESTS=ON` | OK |
| Debug build | `cmake --build --preset win-amd64-debug` | OK, 1,433 build steps from scratch |
| Debug tests | `ctest --preset win-amd64-debug --output-on-failure` | 1,823/1,823 passed (118 s) |
| Release build | `cmake --build --preset win-amd64-release` | OK |
| Release tests | `ctest --preset win-amd64-release --output-on-failure` | 1,823/1,823 passed (91 s) |
| Install | `cmake --install out/build/win-amd64 --config Release` | OK, to `out/install/win-amd64` |

The run needed no step beyond the README. This proves the documented commands on
this machine; the machine already had Visual Studio, LLVM, CMake, Ninja and the
Windows SDK installed, so it is not a fresh-PC result.

## Default-switch gates for the native backends

**Decision, 2026-09-28:** the repository owner made the native backends the
defaults (GameInput in GDK builds and XInput otherwise, the Win32 window,
XAudio2) ahead of the remaining gates below, and decided that SDL is to be
removed. The gates stay as the record of what is and is not yet proven; the
status line below says which are open.

These were the gates for a native backend to become the default:

1. The backend's unit tests and fixtures pass in Debug and Release.
2. At least two recorded title soaks use it with no new fatal, error or
   missing-function log lines against the SDL run of the same scene, using
   `scripts/capture_baseline.py --run-for`.
3. **Input:** controlled input changes game state (007 #16), and connect,
   disconnect and reconnect work on real pads.
   **Window:** resize, minimize, restore, fullscreen and DPI change work in a
   title.
   **Audio:** device change and unplug work in a title.
4. The fallback still works: a missing runtime falls back to SDL with a logged
   diagnostic (GameInput already does).

SDL3 was removed by RG-GDK-033 (issue #97) on 2026-09-28, with every consumer in
the [SDL ownership map](windowing.md) replaced.

Status on 2026-09-27: gate 1 met for all three. Gate 2 has one title soak each
(Quantum of Solace native run). Gates 3 and 4 are partly met: GameInput falls
back to SDL; in-title device and window-state tests are not run.

## Rollback

Every native path is selected at startup and can be undone without a rebuild:

| Change | Roll back with |
| --- | --- |
| GameInput | `input_backend = "xinput"` |
| SDL3 removal (Win32 window, XAudio2, XInput) | None by configuration: revert the RG-GDK-033 PR |
| Gaming Runtime at startup | `gaming_runtime = "off"` |
| Real ZPD occlusion counts (RG-GDK-010) | `occlusion_query` cvar (see [regression strategy](regression-strategy.md)) |
| GDK build | Build the standard `win-amd64` preset; it needs no GDK |

Removals are not rolled back by configuration. Vulkan, Linux and macOS were
removed by RG-GDK-023 and RG-GDK-024; restoring them needs a revert of those
PRs and a new ADR.

## Continuous integration

- Pull requests run `Lint & Format Check`: clang-format, plus the script tests in
  `scripts/tests`, which include `check_docs.py` (links and paths in these
  handoff documents).
- The Windows build (`build-win-amd64.yaml`) runs only for `v*` tags and does not
  run CTest.
- The nightly workflow is manual-only. Its inherited schedule used a
  `development` branch this fork does not have and failed daily.
- Unit, PPC, GPU fixture, kernel and GDK tests therefore run on the development
  machine, as recorded in each issue. Adding CTest to CI would need a Windows
  runner with the bundled PPC toolchain and, for GDK tests, a GDK installation.

## Guide audit and title rebuilds, 2026-10-07

The [issue audit and rebuild record](guide-issue-audit-20261007.md) records
current Guide issue states and private package-layer checks. SDK source
`34ab1402c5e0428679a8cc239747211a865f1549` with Guide
`db55a4d347c9edc74a17632fd7f71311ea1e7410` builds completely in GDK 260404
Release on the existing Windows x64 / Clang 22.1.8 / VS Community machine.
The SDK was installed to an isolated `out/bc-title-sdk-gdk` prefix; its temporary
CMake user-registry entry was removed without changing other registrations.

Targeted GDK tests with the owner's BC Flash resources pass 54 cases / 662
assertions, covering reusable message boxes, sources/media, title-update
validation, launch settings and GDK adapters. A separate run including the
live Xbox Unity download case fails on WinHTTP timeout `12002`; 52 cases pass,
two asset cases skip and one network case fails. The offline rerun explicitly
excludes `[.network]` and enables the asset cases. The live service failure is
not treated as a passed download gate or evidence of a code regression.
Logs are under `out/bc-title-rebuild-20261007`.

Five title projects were regenerated and built in isolated local staging
folders, including Quantum of Solace TU2 and FIFA Street's two guest modules.
The old source XEX hashes match the title repository records; prior executable
hashes are captured in `inputs-and-baselines.json`. All five generation,
configuration and build results are zero. Artifact checks pass for six EXEs:
large/small Windows icons, matching SDK runtime/GPU DLL hashes, source identities,
Guide bundles, launch defaults, both FIFA guest modules and the retained QoS
shader cache. All six baseline EXE hashes are unchanged. Results and output
locations are recorded in the linked audit. No staged game was launched;
existing title status and gameplay/vendor gates
remain unchanged by compilation.


## Guide presentation correction, 2026-10-07

The owner clarified that the original Xbox 360 Guide appearance must remain,
with PC compatibility features added behind it. The staged builds had enabled
the optional ImGui launch settings and preferred the installed Fuzion Frenzy
Flash assets. Both choices were outside that presentation requirement.

All five staging projects were reconfigured with `REXGLUE_GUIDE_FLASH` empty
and rebuilt successfully, including both QoS executable variants. They now embed
the original 2.0.17559 system-update assets. Adjacent settings for the six staged
EXEs and disposable test copies explicitly set `launch_menu=false`; the compiled
experimental title default remains overridable. The local probe defaults to
direct launch and requires `--menu` to request the experimental settings screen.
Logs/results are in `out/guide-presentation-restore-20261007`.

The initial disposable QoS process is no longer running. Interactive testing
was stopped at the owner's correction; no painted Guide or gameplay pass is
claimed. Existing installations and saves were not replaced or modified.
See the [presentation contract](pc-backward-compatibility.md#presentation-contract).


## Guide selection by title host, 2026-10-07

[ADR-015](adr/ADR-015-guide-presentation-by-title.md) records the explicit
Xbox 360/Original Xbox presentation contract. The Guide defaults to Xbox 360
rather than choosing emulator scenes from available assets. The SDK target
helper selects and embeds the corresponding source; BC Flash no longer affects
360 builds. Both embedded and external asset loads retain the host's selection.

The GDK Release SDK builds successfully. Final targeted offline tests pass in
standard and GDK Debug/Release: **49 cases**, **826 assertions in Release** and
**824 in Debug** for each SDK variant. Source/recovery controls and empty/running
activity scene models are exercised with both private asset sets. Standalone
Guide Debug/Release each pass one CTest containing five cases / 48 assertions.
An initial broader concurrent run included the live Unity test and encountered
a shared temporary-directory file lock. Final suites run sequentially and
exclude `[.network]` in every comma-separated Catch filter clause; this does not
establish a live-download gate. Logs are in `out/guide-platform-20261007`.

Installed-SDK consumers configure and link for both presentations. Bundle hashes
match separately generated console-only and BC-plus-console reference bundles;
target definitions select Original Xbox only for the explicit BC host. Invalid
presentation and missing BC Flash configurations are rejected. The reusable
consumer is `tests/consumer/guide_presentation`, with the cache option
`TEST_GUIDE_PRESENTATION`; its inherited launcher probe is a linking fixture,
not an original Xbox title execution test.

All five local staging projects explicitly select Xbox 360 and rebuild
successfully, including QoS TU2 and FIFA guest modules. Original Guide bundles
and direct-launch config overrides remain in their six EXEs' folders; matching
runtime DLLs are copied into the disposable test folders. Prior installations
and saves remain unchanged. Rebuild logs are in
`out/guide-platform-20261007/title-rebuild`.

A disposable QoS run mounts the full `baseline/extracted-20260923` game directory
read-only and uses isolated settings/cache/user data. Its original XEX hash
matches the codegen input. The earlier probe incorrectly mounted the codegen-only
`material-20260923` directory, causing missing-content/self-relaunch termination;
correcting the test mount resolves that setup error. On NVIDIA RTX 5080 Laptop,
driver **32.0.16.1742**, client captures verify original Guide opening/tab
navigation, a populated 50-achievement grid, exit confirmation and empty Active
Downloads. Testing exposed notification controls left visible when the activity
count initially equals zero; shared scene preparation fixes this and the painted
recheck passes. Screenshots/logs remain private under
`out/title-ui-validation-20261007/quantumofsolace`. Software key events are not a
physical-controller acceptance test. This driver differs from the earlier
32.0.16.1714 baseline. Other title playthroughs, TU2 execution, physical input,
AMD/Intel and 8K checks are not established here. Owned test processes were closed.

Original Xbox presentation uses owner's BC scenes but is not a 1:1 fidelity
claim or an execution backend. Original Xbox CPU/kernel/media execution, Xbox
account linking, entitlements, cloud saves and live services still require their
separate implementation/design/deployment gates. No issue is closed by this work.


Follow-up staged window probes also launch Blood Stone, Legends, FIFA Street
and NHL with disposable data. Blood Stone's original Guide is visibly present.
Legends' initial captured Guide is partially clipped during the startup probe;
its settled/fidelity result is unresolved. Earlier four-second captures were
black startup frames and are not passes. Subsequent Legends/FIFA/NHL capture
attempts cannot acquire the owned foreground window and therefore refuse
input/screenshots. No painted pass is claimed for those three titles. Capture
records are `title-window-checks.json` and `title-window-checks-final.json` in the
same private evidence folder. All owned processes are closed. The current
[Guide draft PR](https://github.com/furqanagwan/xbox/pull/9) Windows CI checks
pass, as do the [SDK draft PR](https://github.com/furqanagwan/rexglue-sdk/pull/215)
format checks; these do not replace the missing owner/hardware/service gates.

## PPC instruction coverage follow-up, 2026-10-07

Candidate source: `b8d57d4845052d603d0532944f56af57b37a55d3`, PR #162,
integrating main `5dad97248d70126c901d2425ce942e5dc9694a28`. Windows x64,
VS Community 18.10.3, Clang 22.1.8, April 2026 GDK 260404; NVIDIA RTX 5080
Laptop, driver 32.0.16.1742 for the existing GPU fixtures and title starts.
This is CPU instruction work; no shader path/default or vendor support changes.

- Standard Release full build/CTest: 2,214 discovered, 2,199 passed,
  15 skipped, zero failures, 216.88 s. Logs: `out/ppc149-standard-release-ctest.log`
  and `out/ppc149-win-amd64-release-retry-build.log`.
- Standard Debug full build/CTest: 2,214 discovered, 2,199 passed,
  14 skipped and the previously reproduced quad tessellation assertion,
  236.18 s. All discovered unit and ordinary PPC tests pass or skip.
  Logs: `out/ppc149-standard-debug-{build,ctest}.log`.
- GDK Debug and Release full builds, then unit/PPC/corpus CTest selection:
  each discovers 2,693 entries, 2,679 passed, 14 skipped, zero failures
  (125.14 s Debug, 87.94 s Release). These are CPU/unit selections, not
  new full GDK GPU/deployment results. Logs:
  `out/ppc149-gdk-{debug,release}-ctest.log` and associated build logs.
- Direct ordinary PPC checks pass 1,575 cases / 6,518 assertions in both
  GDK configurations and the final standard Release recheck. The pinned
  corpus generator emits 169,459 cases, reports 917 skipped, and all 563
  active Catch2 file groups pass. Four wholly skipped files account for the
  difference from the 567 registered CTest file entries. All 3,333 #149
  expected-failure entries are removed; eight hand-written corpus errors
  remain expected. The corpus and skip-list source pin are unchanged.
- CPU/context unit selection passes 23 cases / 99 assertions. Non-volatile
  restoration now verifies that guest-written FPSCR bits survive alongside
  the host-backed control word. MSR tests cover both write instructions,
  nested disable/restore, repeated enables and case isolation. The new
  `mcrfs` checks retain RN and result flags while clearing exception flags.

An exploratory all-in-one GDK Debug `unit_tests.exe` invocation aborts at
`Memory::Memory`'s `active_memory_ == 0` assertion. A baseline combined run was
not established, so it is not classified as a pre-existing regression. The
supported CTest invocation isolates each unit case in its own process and all
551 GDK unit entries pass or skip. The abort is retained in
`out/ppc149-gdk-debug-unit.log`; it is not reported as a passing combined run.
An initial filtered CTest attempt also encountered stale kernel test imports
after rebuilding only selected targets; the subsequent full GDK Debug build
and successful discovery/run above replace that incomplete-build result.

Fresh title generation/builds use separate `recompiled-ppc149-20261007`
projects and the isolated SDK install `out/ppc149-title-sdk`. The original
five XEX inputs and six existing installed EXEs match their prior recorded
SHA-256 values (`out/ppc149-original-artifact-checks.json`). Previous staging
builds are kept for paired startup checks. Each process receives a read-only
game root, separate user/cache/log paths and `launch_menu=false`.
Private logs and generated/game material are not distributed.

Title results are recorded under `out/ppc149-title-rebuild` and
`out/ppc149-title-soaks`. Paired runs observe startup, advancing GPU-frame
reports and controlled closure; they are not gameplay, save/load, visual
comparison or performance benchmarks. Concurrent build activity means frame
counts cannot establish a performance improvement. The QoS TU2 executable is
a build result, not a validated TU2 playthrough.

| Title | Fresh generation/configure/build | Previous/candidate startup |
| --- | --- | --- |
| Quantum of Solace (base and TU2 built) | Pass | Base: both 90 seconds, advancing frames |
| Blood Stone | Pass | Both 90 seconds, advancing frames |
| 007 Legends | Pass | Both 90 seconds, advancing frames |
| FIFA Street (including two guest DLLs) | Pass | Both 90 seconds, advancing frames |
| NHL Legacy Edition | Pass | Both 90 seconds, advancing frames |

All ten runs retain a visible window through the observation interval, exit
with status zero after controlled closure, and require no forced cleanup.
The log scan finds no fatal error, unimplemented PPC instruction, missing
function or device-removal message during these intervals. This does not
establish that unvisited code paths are supported. FIFA Street's generation
retains its previous unresolved-function/heap warnings; NHL retains its
previous large-function warning. Known FIFA crowd-colour and NHL RTV rendering
issues are not claimed resolved. QoS TU2's previously recorded missing
function remains outside the base-title run. The private paired frame record
is `out/ppc149-title-soaks/frame-evidence.json`.

Arithmetic-produced FPSCR exception causes and derived summary bits remain
unimplemented; `mcrfs` is verified for guest-written fields. The existing
modeled MSR mask and critical-region adapter are not a complete guest interrupt
controller. These limits and representative gameplay gates keep #149 open;
instruction-corpus success does not certify every title or opcode. The
provenance and reviewed upstream regression are in
[the tracking ledger](upstream-tracking.md#rg-gdk-053-main-integration-and-corpus-follow-up-2026-10-07).

## PPC arithmetic FPSCR follow-up, 2026-10-08

On Windows x64 with April 2026 GDK 260404 and the Visual Studio developer
shell, Debug and Release `unit_tests` and `ppc_tests` targets build. Eighteen
PPC arithmetic/compare/conversion/estimate cases, three existing `mcrfs` cases
and one unit case pass under CTest in each configuration. The full ordinary PPC
executable passes 1,593 cases and 6,550 assertions in both Debug and Release.
This is not a full CTest or title validation. FPSCR result-class fields and
other estimate effects remain outside this evidence. See the
[upstream ledger](upstream-tracking.md#rg-gdk-053-arithmetic-fpscr-follow-up-2026-10-08).

The generated GDK Debug corpus was rebuilt after the scalar FPSCR changes.
All 566 active CTest groups pass when the existing `instr_mtmsrd` timeout case
is excluded. Corpus validation caught and drove fixes for fused subtract
infinity cancellation, NaN product classification, and preserving guest-written
FEX during `mtfsf`. This result is corpus evidence only; `instr_mtmsrd` remains
an unresolved timeout and gameplay validation remains outstanding.

The Release SDK install from `fc59fce` also regenerated and built the base
Quantum of Solace, Quantum of Solace TU2, Blood Stone and 007 Legends targets
in fresh ignored directories under the 007 repository. Their executable
SHA-256s are, respectively, `1941E6B19839367A9B9C2062A3E7F82076A181BA5834F5114B2F665C80834241`,
`F30937215738BEE9242E3CA8F0F514619704E8D7512DF432EDCAF4C59376CB28`,
`FEC03869A9C702372EA2A4F8CDA9F85A0C81CB3FFE626D5CC3394C6B4FB46E78` and
`3D2643D3EC1F0CE1F6D02794F1EFF36F30E0BF8C9798611AF3EA7BBFB6320246`.
Runtime checks were not run: no `REXGLUE_SYSTEM_UPDATE` or local fallback
bundle was present, so these builds do not embed the Guide required for the
usual Guide exit flow. 007 Legends codegen also reports its existing
1,437,835-byte function above the 1 MiB limit. The build is not runtime or
compatibility evidence.

## Native title artwork follow-up, 2026-10-08

`scripts/test_title_art.py` passes using synthetic artwork: the external CMake
consumer embeds base/update ICO resources, Shell extracts both icon sizes,
resource inspection finds RT_ICON/RT_GROUP_ICON and the linker manifest, the
five PNG outputs have the requested dimensions, and overwrite protection
preserves caller-owned files. The base/TU2 Quantum of Solace, Blood Stone and
007 Legends executables built from the isolated SDK install also pass the
native Shell/resource check with their title artwork. Xbox PC Add game
appearance is unverified, so issue #164 stays open; no installed title or
Xbox library entry was changed. `scripts/validate_gameconfig.ps1` passes
against the installed 260404 `GameConfigSchema.xsd` for minimal and supplied-ID
configs, verifies required image dimensions, refuses silent identity overwrite
and confirms the schema validator rejects an invalid version.

## Game source and media recovery review, 2026-10-08

The focused source-selection/XDVDFS/media-recovery CTest selection passes in
GDK Debug and Release (13 entries each; one private Guide message-box test is
skipped because `$SystemUpdate` is unavailable). It covers XEX identity and
checksum checks, extraction without overwriting existing data, XDVDFS reads
and short reads on open files, replacement-media rejection, retry/leave
behavior, shutdown cancellation and first-run UI validation. The optical-drive
and ISO/folder picker flows were not driven end to end, and physical same-disc
removal/reinsert was not exercised, so issue #154 remains open; synthetic
file failures do not establish hardware drive behavior.

## Required GDK maintenance and ecosystem audit, 2026-10-07

Branch `sdk-gdk-only-maturity` starts at SDK main
`5dad97248d70126c901d2425ce942e5dc9694a28`, without the separate PPC PR #162.
[ADR-016](adr/ADR-016-required-pc-gdk.md) requires PC GDK 260404 and makes
`win-amd64` an alias of the canonical GDK build/install directories. This is
a build-policy/CI/ownership/handoff change, not a GPU port or title compatibility
improvement. The [six-repository audit](sdk-maturity-audit-20261007.md) and
[Edge GPU comparison](edge-gpu-review-20261007.md) pin their review scope.

Local toolchain: Windows x64, installed PC GDK 260404, Clang 22.1.8,
VS Community 2026 18.10.3 developer shell, Windows SDK 10.0.26100.0, Ninja
Multi-Config. Community remains observed compatibility, not a Microsoft-supported
pairing. Configure and full builds succeed in Debug and Release.

- Release CTest `-L 'unit|ppc'`: 2,591 selections, 2,577 passed, 14 skipped,
  zero failures, 74.54 seconds. Includes the 567-file corpus and installed-runtime
  cases. Existing expected-failure/unsupported corpus lists still apply; this
  result does not mean all hardware instruction expectations pass.
- Debug CTest `-L '^(unit|ppc)$'`: 2,024 selections, 2,010 passed, 14 skipped,
  zero failures, 83.99 seconds. Includes installed-runtime cases. The seven
  installed-runtime tests retain their local gate; hosted CI excludes that label
  while keeping pure GDK ABI/mapping tests.
- The initial broader Debug selection did **not** complete. It stopped at
  `ppc_corpus.instr_mtmsrd` after 2,313 completed selections with a runtime
  `abort()` dialog. The generated instruction releases the global lock with no
  matching acquisition in its standalone fixture; the test template asserts
  that contract. This is a likely cause, not a debugger-confirmed stack trace.
  Owned CTest/child processes were terminated. No assertion is removed, no
  corpus skip is added, and no full Debug corpus pass is claimed.
- GDK-off, nonexistent-root and wrong-edition configurations reject with the
  expected diagnostics. The compatibility alias configures the GDK tree and
  emits `compile_commands.json`. An initial alias check overlapped the Debug
  build and hit Ninja's file lock; the serialized retry succeeds.
- A fresh Release installation configures and links the external
  `tests/consumer/launch_settings` consumer against its own GDK selection, with
  CMake package-registry search disabled. Missing consumer GDK is rejected.
  Only the disposable install's registry entry was removed afterward.
- `scripts/setup_gdk.ps1 -InstalledOnly` and PowerShell parsing pass. Formatter
  20.1.8 checks the changed C++ files. Script suite: 13 tests plus nine subtests
  pass. Roadmap validation, handoff links and `git diff --check` pass.

Private logs use the `out/maturity-*` prefix. No new game execution, GPU/vendor,
deployment, physical-controller or save/load gate is established. The prior
Debug GPU quad-tessellation assertion remains unresolved and was not rerun
for this non-GPU change. No issue is closed by this batch. The local setup check
uses the installed GDK; public-payload extraction and hosted software results
are recorded below. Helix hardware support is not established.

The first hosted run, [37690858740](https://github.com/furqanagwan/rexglue-sdk/actions/runs/37690858740),
successfully downloads, digest-checks and extracts the public GDK, configures,
builds Debug and runs 2,017 software selections. It reports one failing identity
fixture: `TEMP` and the CTest working directory are on different drives, so
`std::filesystem::relative` cannot represent the fixture's path. Keep that
relative-path fixture under the CTest working directory and require a fresh
directory before writing. No production source validation is weakened and no
test is excluded. The corrected workflow's complete result is recorded below.

## Primitive conversion cache correctness, 2026-10-07

Branch `gpu-primitive-cache-invalidation` builds on GDK-policy commit
`99d4c0a8db459f2203dffc7049ced534576d3ad2`. Exact Edge source, associated-PR/comment
review, classification and the local locking adaptation are in
[the tracking ledger](upstream-tracking.md#primitive-conversion-cache-invalidation-2026-10-07).
The production patch affects only cache overlap/in-flight insertion, shared
by the D3D12 shader paths; no shader/default/guest execution rewrite.

Baseline CPU tests fail four of six initial cases against the original converter.
Final seven cases pass **494 assertions** each in Debug and Release, both through
isolated CTest and the dedicated single-memory test binary. They cover exact
overlap shapes, 16/32-bit and two-bucket ranges, coarse writes, in-flight writes,
unrelated/adjacent/empty controls, separate entries in one bucket and actual
physical-memory callback dispatch with changed converted contents.

On NVIDIA RTX 5080 Laptop / driver **32.0.16.1742**, the new triangle-fan GPU
readback fails with the original source on **both RTV and ROV**: after writing
degenerate indices, the result stays `0xFFFFFFFF` rather than background
`0xFF0000FF`. With the patch it passes both paths in Debug and Release. Existing
GPU selections also pass: **57 Release**, **56 Debug**; Debug explicitly excludes
the previously recorded quad-tessellation assertion. PIX capture is excluded;
DXIL parity is a separate gate. AMD/Intel remain untested per ADR-007.
The optional GDK/DXIL Release plugin also builds and the new readback case
passes both paths with `gpu_shader_path=dxil` and
`gpu_shader_path_dxil_strict=true` (18 assertions); this is the affected case,
not a fresh full DXIL parity run. Its initial CTest discovery hit an older
unit executable with exit `0xc0000139` after the runtime rebuild; rebuilding
that tree's unit/primitive targets fixes discovery and the final CTest passes.
During the temporary baseline swap, restoring an older source timestamp initially
left Ninja's old plugin in place; refreshing the timestamp and rebuilding both
configurations restores the passing candidate. No production assertion is disabled.

Focused Clang-Tidy analysis of the new CPU adapter exposed naming and widened
allocation arithmetic, which were corrected. The final run has seven
`bugprone-throwing-static-initialization` diagnostics from Catch2's registration
macros; this is not a repository-wide clean analyzer baseline. Formatting,
roadmap and handoff-document checks pass.

The three private 007 staging projects rebuild once against the candidate GDK
installation: QoS (its existing TU2 build target also links), Blood Stone and
Legends. Existing generated code is reused because this fix does not change
codegen. The configured dashboard source path was no longer present; private
modules recovered from the retained console-only bundle allow the rebuild
without substituting BC assets. All three rebuilt Guide bundles retain SHA-256
`d6aec228966231e6f5a4de2926507e23a5a2c48ffca9989bd9896e50d51bba30`.

One candidate instance per title stays alive for a **30-second** startup smoke,
uses a borderless fullscreen window matching the **1280x720** monitor
(`0x16000000`, no caption/frame), and closes through the owned window with status
zero. No fatal/assert/missing-function/device-removal diagnostics were found;
combined runtime diagnostics total **2,470 bytes**. Settings/cache/user data are
isolated, game-relative writes disabled, and existing saves/patch choices are
preserved. Fullscreen/startup is established; painted scenes, Guide exit,
physical controller, gameplay and save/load are not established by this smoke.

Compact private rebuild/input/hash/smoke records are in
`out/primitive-title-batch`; validation logs initially use `out/primitive-*`.
There is no measured performance superiority claim and no issue closure.

## Hosted validation and evidence cleanup, 2026-10-08

The corrected GDK maintenance commit
`99d4c0a8db459f2203dffc7049ced534576d3ad2` passes
[run 37692972322](https://github.com/furqanagwan/rexglue-sdk/actions/runs/37692972322).
Debug and Release each select **2,017** tests: **1,999 passed, 18 skipped,
zero failures**, in 155.29 and 83.44 seconds respectively. The primitive-cache
implementation commit `60b761eeda2d36e93538f2d4fcc2f809197cad0f` passes
[run 37695313281](https://github.com/furqanagwan/rexglue-sdk/actions/runs/37695313281).
Both configurations select **2,024** tests: **2,006 passed, 18 skipped,
zero failures**, in 108.20 and 61.37 seconds respectively. The seven added
primitive cases run in both configurations.

These runs download, SHA-256-check and extract the pinned public GDK 260404,
then build with Clang 20.1.8 on the Windows VS2026 hosted runner. Its observed
VS Enterprise toolchain is additional CI evidence, not a Microsoft-supported
pairing claim. The seven installed-runtime cases remain excluded on hosted CI;
the 18 skips include private console asset and audio fixtures. Local installed
runtime checks above remain separate. Hosted checks do not establish GPU,
title gameplay, deployment, controller or save compatibility, and do not resolve
the broader Debug PPC corpus abort or Debug GPU tessellation assertion.

After extracting compact results, **66** disposable logs from this batch were
removed, reclaiming **28,050,710 bytes (26.8 MiB)**. Only explicitly scoped
`out/maturity-*.log`, `out/primitive-*.log` and logs under
`out/primitive-title-batch` were removed. Private JSON input hashes, title
rebuild/smoke records and cleanup manifests remain, including
`out/sdk-maturity-results-20261008.json` and
`out/sdk-log-cleanup-20261008.json`. Binaries, inputs, saves, preferences and
reference worktrees were preserved. Earlier log cleanup is a separate record;
these figures count only this batch.

## getBCF readback validation, 2026-10-08

On the NVIDIA RTX 5080 Laptop GPU (driver `32.0.16.1742`) and Intel Graphics
(driver `32.0.101.6129`), the two synthetic getBCF GPU cases pass with 290
assertions in Debug and Release. Both default DXBC and strict DXIL paths pass on
each adapter. RTV and ROV paths, point/linear filtering, signed textures, mips,
volumes, stacked textures and cube controls are covered. These are synthetic
shader readbacks, not title gameplay or broad vendor certification. The Debug
tessellated-quad assertion `register_count() >= 2` also reproduces separately;
its baseline and relationship to this work remain unknown.

## VIZ_QUERY focused regression check, 2026-10-08

On the NVIDIA RTX 5080 Laptop GPU, the two synthetic VIZ consumer-draw tests
pass in GDK Debug and Release (2/2 in each configuration). They cover a survey
hidden behind depth skipping its consumer draw and a fully visible survey
keeping its consumer draw. The tests use the SDK's synthetic fixture; they do
not establish that a representative 007 title issues `VIZ_QUERY` or exercises
conditional rendering. Issue [#65](https://github.com/furqanagwan/rexglue-sdk/issues/65)
therefore remains open for title-level evidence.

## FPSCR result classification, 2026-10-08

The PPC floating-point helpers now report the defined FPRF result classes for
tracked arithmetic results and the FPCC relation for floating-point compares.
Classification uses the instruction's result precision, including single
subnormals held in widened FPRs. Compares preserve C, FR and FI; enabled invalid
operations preserve FPRF. The implementation follows the field encodings and
exception behavior in the [PowerPC User Instruction Set Architecture v2.02].

After rebuilding `unit_tests` and `ppc_tests`, the focused unit, FPSCR
arithmetic and FPSCR state selection passes 28/28 CTest cases in GDK Debug and
Release. The full ordinary PPC selection passes 1,595/1,595 cases in each
configuration. The full unit selection passes 542 cases with 14 documented
private asset/fixture skips in each configuration. These are synthetic checks.
Estimate-instruction classification followed on 2026-10-09 (below);
title/gameplay validation remains separate.

The FR/FI follow-up verifies rounding direction and inexact status for scalar
arithmetic, fused operations, square root, `frsp`, integer conversion and
integer-to-double conversion. Directed rounding and ties are covered by unit
tests. Representative-title validation remains open.

The 2026-10-09 estimate follow-up sets FPRF for `fres` and `frsqrte`, records
`fres` underflow for operands above 2^126, and preserves FPRF when an enabled
invalid or zero-divide exception suppresses the result. GDK Debug and Release
each pass the focused FPSCR selection (29/29 CTest cases) and the ordinary PPC
executable (1,599 cases / 6,559 assertions). These are synthetic checks;
hardware confirmation of `fres` underflow signalling is unavailable.

[PowerPC User Instruction Set Architecture v2.02]: https://powerpc.dev/general/PPC_Vers202_Book1_public.pdf

## PR #162 corpus and 007 title batch, 2026-10-09

Candidate: PR #162 at `51aeb8d` (estimate FPRF, with `main` merged in).

- **Corpus:** the GDK Release PPC corpus, regenerated by this branch's
  `rexglue`, passes **567/567** CTest groups with nothing excluded, including
  `ppc_corpus.instr_mtmsrd`. The earlier `instr_mtmsrd` hang and Debug abort came
  from a corpus build generated by a `main`-based `rexglue` (before the
  EE-transition `mtmsr` lowering), whose code released the interrupt lock
  without taking it. The regenerated case follows EE transitions; the
  identical ordinary `tests/ppc/asm/instr_mtmsrd.s` passes in GDK Debug. A full
  Debug corpus run was not repeated.
- **Titles:** the SDK was installed to a separate candidate prefix. Quantum of
  Solace (base and TU2 targets), Blood Stone and 007 Legends were regenerated
  with `rexglue codegen` and built in private candidate copies; the owner's
  working builds and saves were not touched. The Guide was built from the
  retained console-only modules; all three bundles keep SHA-256
  `d6aec228966231e6f5a4de2926507e23a5a2c48ffca9989bd9896e50d51bba30`.
- **Startup:** one fullscreen candidate run per base title, with an empty,
  disposable user-data folder, stayed alive for **90 seconds** and closed
  through its window without forced cleanup. Logs at warning level hold no
  fatal, assert, missing-function or device-removal entries. The only
  warnings are already-known ones: Quantum of Solace's language-file probes,
  Blood Stone's localisation folder probes and Legends'
  `XamContentResolve` (see the upstream ledger). Blood Stone wrote checkpoint
  and profile saves and Legends an autosave into the disposable folder.

| Title | Executable SHA-256 |
| --- | --- |
| Quantum of Solace | `d9520ac4b969b252e8603b6357e73a82a5c683dd6b73ae79183f5a5291402d24` |
| Blood Stone | `5458ea02dd28b4a03dfb5e9f4d0b74eb350a9a9acfa96f24bb2022ec7a3fc5d8` |
| 007 Legends | `ac0fa16ba6fe2992542dd65f1a46ce68647b34b54e18c753aabe7fd577a183b7` |

Runtime `rexruntime.dll` `524a9ceb38554d3934820207095690a0c6b0af1b8680cacc271cf4b157466ed7`,
`rexgpu-xenos.dll` `21fb4c8bb8537074a04ebdbce7d83377482df7760be234d9a0586399e36a6520`.
This is a startup observation without input: gameplay, Guide exit, save/load
and visual correctness are not established, and the TU2 target was built but

## Achievement popup display freeze, 2026-10-09

Owner report: in Quantum of Solace the picture froze for good when an
achievement unlocked, while sound and controller input kept working.
Reproduced without input on a candidate build: "Licence to Kill" unlocks about
40 seconds in, after which screenshots stop changing while guest frame pacing
stays near 60 fps. Temporary logging showed the cause. The popup's first
frame creates the Guide media, including its XAudio2 engine, inside a UI
paint. That setup pumped window messages, and `Window::OnPaint` dropped the
nested `WM_PAINT`. The Presenter requests a paint only when none is pending,
so its pending flag was never cleared and painting never resumed.

`Window::OnPaint` now remembers a paint that arrives during painting and
requests it again when the outer paint ends. With only this change in
`rexruntime.dll`, the same run keeps presenting through the unlock and shows
the popup ("Achievement unlocked / 10G - Licence to Kill"). One reproduction
and one fixed run were compared; other titles and the original-Xbox popup were
not run.

## Shader parsing, DXIL occlusion and wide 1D title batch, 2026-10-09

SDK `6c20b62` (PRs #238, #239, #240), GDK Release, NVIDIA RTX 5080 Laptop.
Each title built twice from one codegen: against the default SDK install
and against the `REXGLUE_SHADER_DXIL` install (run with
`gpu_shader_path=dxil`). One fullscreen run each, 90 s from launch with no
input, fresh profile and cold shader cache, frame stats every 5 s and a
screenshot every 4 s.

| Run | Exit | Errors | Median fps | Longest frame | 5 s windows with a frame over 300 ms |
| --- | --- | --- | --- | --- | --- |
| Quantum of Solace, DXBC | clean | 0 | 56 | 6.3 s (level load) | 1 |
| Quantum of Solace, DXIL | clean | 0 | 43 | 8.8 s (level load, pipelines created cold) | 5 |
| Blood Stone, DXBC | clean | 0 | 30 (title's cap) | 412 ms | 1 |
| Blood Stone, DXIL | clean | 0 | 29 | 2.8 s | 1 |
| 007 Legends, DXBC | clean | 0 | 60 | 285 ms | 0 |
| 007 Legends, DXIL | clean | 0 | 59 | 2.2 s | 2 |

Warnings are the titles' usual missing-file probes and kernel stubs, the
same on both paths. No "1D texture is too wide", failed DXIL pipeline or
device-removed messages. The screenshots change throughout every run (the
only repeat is three QoS DXIL shots during its 8.8 s level-load stall) and
show correct rendering on both paths: QoS gameplay, Blood Stone gameplay
and Legends' in-engine cutscenes. Without input the two paths reach
different points by the same time, so this is not a frame-exact comparison.
DXIL's longer frames are the known cold-cache pipeline creation (see
[DXIL shader toolchain](shader-dxil.md)). Raw logs and screenshots were
removed after these numbers were taken.
