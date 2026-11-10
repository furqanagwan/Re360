# Modernization documentation

Start with the [investigation](investigation.md) for the executive assessment,
verified ancestry and source applicability. The [roadmap](roadmap.md) is the
implementation entry point and includes actual GitHub issue links.
The [48-deliverable index](deliverables.md) maps every requested output to its artifact.

| Document | Purpose |
| --- | --- |
| [Investigation](investigation.md) | Ancestry, divergence, source findings, classifications, risks and validation limits |
| [Architecture plan](architecture-plan.md) | GPU/Xenos, GDK/native APIs, dependency/removal and build migration |
| [Upstream review](upstream-review.md) | All current open Canary/Edge items and selected closed/merged/rejected work |
| [Upstream tracking](upstream-tracking.md) | Significant compatibility fix provenance, regressions, adaptations and roadmap mapping |
| [Regression strategy](regression-strategy.md) | Current baseline, representative workloads, vendor matrix and regression index |
| [Roadmap](roadmap.md) | Sequential issue index, dependencies, labels, readiness and first implementation |
| [Native GPU plan](native-gpu-plan.md) | Ahead-of-time shaders, the lean renderer and per-game native renderers, in phases |
| [ReGPU](regpu.md) | The `regpu` GPU plugin: use, wiring, limits, credits; [debugging](regpu-debugging.md) |
| [ADRs](adr/README.md) | Six accepted architecture/policy decisions, with implementation still staged |
| [Source snapshot](upstream-snapshot.json) | Machine-readable upstream status, source paths and mappings |

Root [README](../README.md) describes setup/status and [AGENTS](../AGENTS.md)
defines canonical human/agent handoff. [RELEASING](RELEASING.md) describes the
inherited upstream release workflow; it is not a claim that the future GDK release
gates have already been implemented.
