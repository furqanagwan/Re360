# ADR-013: Xbox Guide has a separate source and issue repository

Date: 2026-10-06. Status: **superseded** on 2026-11-11 by
[ADR-018](ADR-018-re360-one-repository-two-gpu-plugins.md): the Guide's sources and history are back
in this repository under `src/xbox`. Kept as the record of the separate-repository period; its
issues stay at the old repository's links.

## Context

The Guide was embedded in ReXGlue even though other recomp projects may want
its XUI implementation. Its issues need the same ownership boundary as its
sources. ADR-011's scene-driven behavior and private-asset policy still apply.

## Decision

[xbox-guide](https://github.com/furqanagwan/xbox) owns Guide/XUI sources,
headers and tests. ReXGlue consumes an immutable submodule revision and compiles
its adapter into the existing runtime. Installed headers and title embedding
retain their existing interfaces. The standalone scene target supplies parsing,
timelines, layout and rendering without the SDK runtime. The complete Guide's
current ReXGlue host dependencies are explicit; full portability needs adapters.

Guide-owned issues transfer with their history and state. Guest runtime,
codegen, input and content-service issues remain in ReXGlue. No assets or
unrelated backends move. [Extraction record](../xbox-guide-extraction.md) gives
the source revision, issue mapping and validation limitations.

## Validation

Compare the Guide unit suite before and after extraction; build/test standalone
Debug/Release without the SDK; build/test SDK Debug/Release and verify installed
headers and title-source compilation. Real title/pad validation remains a
separate compatibility gate and absent evidence must remain recorded.
