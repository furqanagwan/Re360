# Xbox repository extraction (historical)

The Guide is back in this repository under `src/xbox` since 2026-11-11
([ADR-018](adr/ADR-018-re360-one-repository-two-gpu-plugins.md)). This page records the
separate-repository period; that repository's own record is in
[research/ui/xbox-guide-repository](../research/ui/xbox-guide-repository/extraction.md).

Date: 2026-10-06. Guide sources were maintained in
[furqanagwan/xbox-guide](https://github.com/furqanagwan/xbox), pinned under
thirdparty/xbox. The extraction source is SDK commit
`d1a87b4ef0a09c7a7813ab2a2b27976de01de203`.

The Guide, native XUI sources, public headers and eight unit-test translation
units move to that repository. The SDK retains host integration, guest XAM
services, input, achievement/content managers, CLI bundling and title embedding.
Source and installed consumers keep their existing include paths and controls.
Update submodules before configuring. No Microsoft assets are published.

The adapter compiles the external sources into rexui, then rexruntime, as
before; no additional runtime DLL or duplicate cvar registry is introduced.
Installed SDK headers include the external Guide/XUI headers. The standalone
scene target has no SDK runtime dependency; the complete Guide still needs
host-service adapters for use with another SDK.

## Issues

Guide-owned issues 127, 132, 143, 145, 153, 161 and 166 transfer to xbox-guide
using GitHub's native transfer, preserving state, comments and old URL redirects.
The open Guide epic's outstanding owner pad session stays open.

Issues 137 (resolution), 139 (input/battery), 141 (guest achievement enumeration)
and 154 (disc/ISO loading) stay in ReXGlue. Future title-update codegen changes
also belong here; its Guide page belongs in xbox-guide. Historical issue IDs
remain RG-GDK IDs, and the transfer map supplies the new canonical URLs.

The completed issue URLs are below. Original labels have also been restored
and verified; GitHub did not transfer custom labels without matching definitions
in the destination repository.


| Original SDK issue | Guide issue | State | Comments preserved |
| --- | --- | --- | --- |
| [#127](https://github.com/furqanagwan/rexglue-sdk/issues/127) | [#1](https://github.com/furqanagwan/xbox/issues/1) | open | 0 |
| [#132](https://github.com/furqanagwan/rexglue-sdk/issues/132) | [#2](https://github.com/furqanagwan/xbox/issues/2) | closed | 0 |
| [#143](https://github.com/furqanagwan/rexglue-sdk/issues/143) | [#3](https://github.com/furqanagwan/xbox/issues/3) | closed | 0 |
| [#145](https://github.com/furqanagwan/rexglue-sdk/issues/145) | [#4](https://github.com/furqanagwan/xbox/issues/4) | closed | 0 |
| [#153](https://github.com/furqanagwan/rexglue-sdk/issues/153) | [#5](https://github.com/furqanagwan/xbox/issues/5) | closed | 1 |
| [#161](https://github.com/furqanagwan/rexglue-sdk/issues/161) | [#6](https://github.com/furqanagwan/xbox/issues/6) | closed | 0 |
| [#166](https://github.com/furqanagwan/rexglue-sdk/issues/166) | [#7](https://github.com/furqanagwan/xbox/issues/7) | closed | 4 |

## Validation, 2026-10-06

Pinned Guide revision: `3d5ea575a0f6a6ceb9f0c39c097d5c06c59b72aa` (the final
change from tested `62267d4` records issue labels and CI evidence only).
Windows x64, Clang 22.1.8, CMake 4.4.3, installed VS 2026 Community.

- Standalone scene library: Debug and Release each discover and pass one CTest
  test containing three Catch2 cases / 16 assertions. Fresh Windows
  [CI at the pinned revision](https://github.com/furqanagwan/xbox/actions/runs/37527134574) also
  builds/tests both configurations after fetching the pinned dependencies,
  without an SDK checkout.
- Refreshed SDK source baseline and extracted Release Guide suites each
  discover 57 cases: 51 passed, six private-asset cases skipped, 583 assertions.
  Extracted Debug and GDK Release builds and Guide suites give the same result.
- Standard Release builds fully; CTest discovers 2,078 tests: 2,067 passed,
  11 skipped, zero failed, including PPC tests and shader reproducibility.
- Standard Debug builds fully; CTest discovers 2,078 tests: 2,066 passed,
  11 skipped, one failed. The tessellated quad GPU fixture asserts
  `register_count() >= 2` at `dxbc_translator.cpp:551`. A fresh Debug build of
  the archived pre-extraction SDK commit reproduces exactly that assertion;
  this is an existing failure, with details in the
  [regression record](regression-strategy.md#debug-tessellation-fixture-2026-10-06).
- Release installs to an isolated prefix. An installed-package consumer
  compiles the Guide, notification and keyboard headers plus shipped ReXApp,
  links the Guide package parser through rexruntime and passes its CTest smoke.
- Documentation links/paths, roadmap validation and diff whitespace checks
  pass. Script tests pass (13 tests plus nine subtests).

Log files are kept locally under `out/guide-*`; no private console/title assets
were used or published. The installed CLI's `guide-bundle --help` also succeeds.
The owner pad/title session and six private-asset Guide cases remain blocked
on owner interaction/material. This extraction adds no new title compatibility
claim, and other SDKs still need complete Guide service adapters.
