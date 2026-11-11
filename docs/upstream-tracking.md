# Upstream tracking and compatibility provenance

## Fuzion Frenzy package-layer verification, 2026-10-07

Read-only reference:
[stocktaylor/Xbox-BC-for-PC-Research b3609bc](https://github.com/stocktaylor/Xbox-BC-for-PC-Research/blob/b3609bc2385860d17d9bc0f8f8eb8020ab84f434/TECHNICAL_FINDINGS.md).
This is class B architecture/host-experience research, with no code port and
no source PR imported. Direct checks of the owner's installed Fuzion Frenzy
package corroborate XEX2 XeFu modules and AMD64 native compatibility DLLs.
The research's missing Emu.exe observation does not apply to this installation.
The [audit record](guide-issue-audit-20261007.md) separates direct header evidence
from inferred translation details and records title scope: 007, FIFA Street
and NHL are Xbox 360 projects, so no original Xbox execution layer is added.
No upstream regression history is inferred from package metadata; no runtime
dependency or proprietary implementation is imported.

## PC host experience, 2026-10-06

Based on local SDK `248d6c3` after the Guide extraction, with read-only package
observations and Microsoft's July 2026 announcement recorded in
[PC backward compatibility coverage](pc-backward-compatibility.md). This is
original host adapter work (class B), not an upstream source port: no Xenia
commit or PR is imported, and no Microsoft implementation/assets are copied.
SDK [#164](https://github.com/furqanagwan/rexglue-sdk/issues/164) is the related
native-art issue and remains open for title deployment/Xbox app gates.

The pre-existing XGetLanguage stub always returned English through a hardcoded
NTSC-U region. A shared resolver now respects configured guest language and an
opt-in Windows locale. XConfig packing/status and XDBF language fallback remain
unchanged. Startup controls use existing cvar contracts before guest setup;
native art uses local supplied images and additive icon resources. Regression
risk is language selection in titles, launcher completion/lifetime and resource
loading; tests cover guest API agreement, locale mapping, UI actions/persistence,
controller navigation/disconnect, icon encoding and native consumer extraction.
The temporary physical-only input reader reuses existing drivers; guest input
factories keep their previous synthetic fallback behaviour. Held input is
consumed through the existing UI handoff contract before guest execution.
Whole-experience parity, real
title/GPU-rendered launcher checks and service integration remain unverified.

No upstream regressions are inferred from this research. The SDK baseline's
known Debug tessellation fixture assertion remains unrelated and open; see
[release evidence](release-evidence.md).

Validation also exposed configuration mixing in vendored Catch2
`88abf9bf325c798c33f54f6b9220ef885b267f4f`: POST_BUILD discovery overwrote a
shared test file with the last-built configuration's executable. The SDK now
selects its existing PRE_TEST discovery mode, without modifying vendored code.
See the [regression record](regression-strategy.md#ctest-configuration-discovery-2026-10-07).

Snapshot: 2026-09-23. These are evaluated candidates, not imported patches. A–H classifications are defined in [the investigation](investigation.md). “Unknown” regressions means none verified during this review, not proven absence. Issue/PR comments contain upstream observations; no local title outcome is inferred. See [roadmap](roadmap.md) for live ReXGlue issue links.

## Compact upstream-to-roadmap index

All implementation statuses are **investigated, not ported**. The roadmap resolves planning IDs to actual issue numbers. Closed-unmerged entries are retained as rejected/research evidence.

| ReXGlue area / upstream item | Source state | Commit (merge or PR head) | Scope/class | Regression risk | ReXGlue issue |
| --- | --- | --- | --- | --- | --- |
| [xenia-canary/xenia-canary #5](https://github.com/xenia-canary/xenia-canary/issues/5) | open | `not identified` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-017 |
| [xenia-canary/xenia-canary #438](https://github.com/xenia-canary/xenia-canary/issues/438) | open | `not identified` | B/H; game-specific; useful research | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #600](https://github.com/xenia-canary/xenia-canary/issues/600) | open | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-019 |
| [xenia-canary/xenia-canary #608](https://github.com/xenia-canary/xenia-canary/issues/608) | open | `not identified` | B/H; regression-related; vendor | See scoped record below; never assume absence | RG-GDK-006 |
| [xenia-canary/xenia-canary #739](https://github.com/xenia-canary/xenia-canary/issues/739) | open | `not identified` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #754](https://github.com/xenia-canary/xenia-canary/issues/754) | open | `not identified` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-014 |
| [xenia-canary/xenia-canary #773](https://github.com/xenia-canary/xenia-canary/issues/773) | open | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-026 |
| [xenia-canary/xenia-canary #872](https://github.com/xenia-canary/xenia-canary/issues/872) | open | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-015 |
| [xenia-canary/xenia-canary #1036](https://github.com/xenia-canary/xenia-canary/issues/1036) | open | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #1093](https://github.com/xenia-canary/xenia-canary/issues/1093) | open | `not identified` | B/H; regression-related; game-specific | See scoped record below; never assume absence | RG-GDK-011 |
| [xenia-canary/xenia-canary #1134](https://github.com/xenia-canary/xenia-canary/issues/1134) | open | `not identified` | B/H; regression-related; vendor | See scoped record below; never assume absence | RG-GDK-006 |
| [xenia-canary/xenia-canary #1214](https://github.com/xenia-canary/xenia-canary/issues/1214) | open | `not identified` | B/H; useful research; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #1220](https://github.com/xenia-canary/xenia-canary/issues/1220) | open | `not identified` | B/H; partly addressed (RG-GDK-017 part 3): close flushes content; the two-user case needs a multi-user profile model | See scoped record below; never assume absence | RG-GDK-017 |
| [xenia-canary/xenia-canary #748](https://github.com/xenia-canary/xenia-canary/pull/748) | open PR | `a7f6514e699018674f5e3f56b437e4d9c58e7641` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #844](https://github.com/xenia-canary/xenia-canary/pull/844) | open PR | `16c13ed6ca60c20e4611e804216f78f5261c99ba` | B/H; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-013 |
| [xenia-canary/xenia-canary #981](https://github.com/xenia-canary/xenia-canary/pull/981) | open PR | `555e9a4a456d2a6d80a8811486208f82095fcfe9` | B; idea adapted (RG-GDK-017 part 3): lock plus shared settings in `UserProfile`; Canary's `UserTracker` has no local counterpart | See scoped record below; never assume absence | RG-GDK-017 |
| [xenia-canary/xenia-canary #1025](https://github.com/xenia-canary/xenia-canary/pull/1025) | open PR | `abcf2ff1bea480cb6c4cdcafd2c09350826e01d3` | B/H; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-015 |
| [xenia-canary/xenia-canary #1077](https://github.com/xenia-canary/xenia-canary/pull/1077) | open PR | `dcd2fff24243b4d2d67c2d08a10d235f04f0de80` | G/H; experimental; not adopted | See scoped record below | RG-GDK-012 |
| [xenia-canary/xenia-canary #1109](https://github.com/xenia-canary/xenia-canary/pull/1109) | open PR | `95f9f68817c9828ba3a28c144916d45d34d44bd1` | B/H; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-017 |
| [xenia-canary/xenia-canary #1111](https://github.com/xenia-canary/xenia-canary/pull/1111) | merged | `692cd59cf` | C/H; adopted behind `occlusion_query_viz` (default off) in RG-GDK-010b | See scoped record below | RG-GDK-010 |
| [xenia-canary/xenia-canary #1182](https://github.com/xenia-canary/xenia-canary/pull/1182) | open PR | `fa6cdaae0f58e9161e5e41ea3683f837b3b112c7` | B/H; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-004 |
| [xenia-canary/xenia-canary #1225](https://github.com/xenia-canary/xenia-canary/pull/1225) | open PR | `fe960bf66f98204940a7464ed35b50ef5b7b4cdc` | B/H; relevant but pending upstream; blocked upstream | See scoped record below; never assume absence | RG-GDK-014 |
| [xenia-canary/xenia-canary #1226](https://github.com/xenia-canary/xenia-canary/pull/1226) | open PR | `c43ea0f9c3e3f3cac600a57d9f464a38c945d808` | B; adapted from the open PR (RG-GDK-017 part 2) | See scoped record below | RG-GDK-017 |
| [xenia-canary/xenia-canary #1230](https://github.com/xenia-canary/xenia-canary/pull/1230) | open PR | `ef97e8a70f4f0d0fffa2736789670f6f8164d055` | B/H; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-020 |
| [xenia-canary/xenia-canary #1016](https://github.com/xenia-canary/xenia-canary/pull/1016) | merged | `fbd620c22b44638b66a70bba80d6f30d55a10924` | C/H; useful research; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-010 |
| [xenia-canary/xenia-canary #1029](https://github.com/xenia-canary/xenia-canary/pull/1029) | closed unmerged | `a6e1a418f33efb87126ba1ffde2b0b536818170a` | B/H; useful research; rejected | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #1031](https://github.com/xenia-canary/xenia-canary/pull/1031) | closed unmerged | `93e3fa59b040f124f0343a80b90f2d0b53b125fc` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-012 |
| [xenia-canary/xenia-canary #1038](https://github.com/xenia-canary/xenia-canary/pull/1038) | merged | `6e5b8324f4101464de0f8c2334edb03cac8826c4` | B/H; regression-related; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-018 |
| [xenia-canary/xenia-canary #1058](https://github.com/xenia-canary/xenia-canary/pull/1058) | merged | `d55670e40b1016cc36cca5111c821b3e7c9a85b8` | B/H; regression-related; behaviour kept by the #1218 port | See scoped record below | RG-GDK-010 |
| [xenia-canary/xenia-canary #1127](https://github.com/xenia-canary/xenia-canary/pull/1127) | merged | `da47dfaacfae1134b238af9083a7f3d413c6cbbe` | B/H; candidate for later port; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-005 |
| [xenia-canary/xenia-canary #1131](https://github.com/xenia-canary/xenia-canary/pull/1131) | merged | `0f2980de442341788c07b282d6fbd6dc689166de` | B/H; useful research; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-012 |
| [xenia-canary/xenia-canary #1135](https://github.com/xenia-canary/xenia-canary/pull/1135) | closed unmerged | `421498c3b38257f98efb043f438b8e28ebf951c9` | G/H; game-specific; watch (no local evidence, RG-GDK-017 part 4) | See scoped record below; never assume absence | RG-GDK-017 |
| [xenia-canary/xenia-canary #1147](https://github.com/xenia-canary/xenia-canary/pull/1147) | merged | `7cd47947b07de30b649fb4224418a659890eab73` | B/H; useful research; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-011 |
| [xenia-canary/xenia-canary #1163](https://github.com/xenia-canary/xenia-canary/pull/1163) | merged | `437a7280cf95310d518a2f68087aab61403956ac` | C/H; candidate for later port; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-009 |
| [xenia-canary/xenia-canary #1180](https://github.com/xenia-canary/xenia-canary/pull/1180) | merged | `22708301ba76d10aae6f7d7caac8b1cac9e4a8e6` | C/H; useful research; redesign | See scoped record below; never assume absence | RG-GDK-004 |
| [xenia-canary/xenia-canary #1190](https://github.com/xenia-canary/xenia-canary/pull/1190) | merged | `3a44f20c7bc66db1da583e8a6f0ab740e31908e9` | B/H; adopted opt-in (off by default) in RG-GDK-012 | See scoped record below | RG-GDK-012 |
| [xenia-canary/xenia-canary #1195](https://github.com/xenia-canary/xenia-canary/pull/1195) | closed unmerged | `ab349b475d4d82dd2a4330b2e2eb46332006afce` | B/H; candidate for later port | See scoped record below; never assume absence | RG-GDK-011 |
| [xenia-canary/xenia-canary #1202](https://github.com/xenia-canary/xenia-canary/pull/1202) | closed unmerged | `1cc288bc71e625bd9272dafc2f1fb01259a62eb2` | B/H; useful research; rejected; regression risk | See scoped record below; never assume absence | RG-GDK-004 |
| [xenia-canary/xenia-canary #1215](https://github.com/xenia-canary/xenia-canary/pull/1215) | merged | `87c24112706d95f15f83dfec58e93923bd7ffa07` | B/H; adopted (adapted) in RG-GDK-004 | See scoped record below | RG-GDK-004 |
| [xenia-canary/xenia-canary #1216](https://github.com/xenia-canary/xenia-canary/pull/1216) | merged | `5d4dc8a88abb2965f2933286571f5bfa0b87391d` | B; adapted (RG-GDK-017 part 1) | See scoped record below | RG-GDK-017 |
| [xenia-canary/xenia-canary #1218](https://github.com/xenia-canary/xenia-canary/pull/1218) | merged | `3d233a5b2e94b940825847b70c788951e364bb33` | C/H; adopted (D3D12 native queries) in RG-GDK-010, ROV counters and RTV hybrid Total in RG-GDK-010a | See scoped record below | RG-GDK-010 |
| [xenia-canary/xenia-canary #1222](https://github.com/xenia-canary/xenia-canary/pull/1222) | merged | `9da693480d0995326b81c3a14f8b1a4c226066eb` | B/H; candidate for later port; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-009 |
| [xenia-canary/xenia-canary #1227](https://github.com/xenia-canary/xenia-canary/pull/1227) | merged | `dcf2994ea1d604e841b5553b2bf941b809e36e79` | B/H; candidate for later port; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-014 |
| [xenia-canary/xenia-canary #1238](https://github.com/xenia-canary/xenia-canary/pull/1238) | merged | `0bd090dbe979bc64959efe519aa320db8e3eb467` | B/H; regression-related; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-009 |
| [xenia-canary/xenia-canary #1240](https://github.com/xenia-canary/xenia-canary/pull/1240) | merged | `89609297c7ae25dc5ba404c6a977260b806bb725` | A/B; adopted (RG-GDK-008) | See scoped record below; never assume absence | RG-GDK-008 |
| [xenia-canary/xenia-canary #1243](https://github.com/xenia-canary/xenia-canary/pull/1243) | merged | `c9c8e483b13249ba5f0060fc4b36071da6d67261` | A/B; adopted (RG-GDK-008) | See scoped record below; never assume absence | RG-GDK-008 |
| [has207/xenia-edge #38](https://github.com/has207/xenia-edge/issues/38) | open | `not identified` | B/H; candidate for later port | See scoped record below; never assume absence | RG-GDK-017 |
| [has207/xenia-edge #234](https://github.com/has207/xenia-edge/issues/234) | open | `not identified` | C/H; regression-related | See scoped record below; never assume absence | RG-GDK-015 |
| [has207/xenia-edge #268](https://github.com/has207/xenia-edge/issues/268) | open | `not identified` | D; useful research; emulator-specific | See scoped record below; never assume absence | RG-GDK-015 |
| [has207/xenia-edge #276](https://github.com/has207/xenia-edge/issues/276) | open | `not identified` | B/H; game-specific; useful research | See scoped record below; never assume absence | RG-GDK-011 |
| [has207/xenia-edge #278](https://github.com/has207/xenia-edge/issues/278) | open | `not identified` | G/H; regression-related; game-specific | See scoped record below; never assume absence | RG-GDK-012 |
| [has207/xenia-edge #280](https://github.com/has207/xenia-edge/issues/280) | open | `not identified` | B/H; game-specific; useful research | See scoped record below; never assume absence | RG-GDK-026 |
| [has207/xenia-edge #251](https://github.com/has207/xenia-edge/pull/251) | open PR | `ecee74cb8d5300bb19ebbba3b53ec6ec2cf76044` | C/H; experimental; relevant but pending upstream | See scoped record below; never assume absence | RG-GDK-015 |
| [has207/xenia-edge #107](https://github.com/has207/xenia-edge/issues/107) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #111](https://github.com/has207/xenia-edge/issues/111) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #113](https://github.com/has207/xenia-edge/issues/113) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #120](https://github.com/has207/xenia-edge/issues/120) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #127](https://github.com/has207/xenia-edge/pull/127) | merged | `397fc9284178b7626e77642c6ea24bfa69a620a8` | B/H; useful research; relevant and merged upstream | See scoped record below; never assume absence | RG-GDK-010 |
| [has207/xenia-edge #141](https://github.com/has207/xenia-edge/issues/141) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #143](https://github.com/has207/xenia-edge/pull/143) | merged | `c9a6327ad136f82920df547afc9e552424b863ac` | G/H; useful research; obsolete workaround; not adopted | See scoped record below | RG-GDK-010 |
| [has207/xenia-edge #145](https://github.com/has207/xenia-edge/pull/145) | closed unmerged | `5d31dea2343fcd60973e17bb89fe0851cca2e1fc` | G/H; experimental; rejected | See scoped record below; never assume absence | RG-GDK-010 |
| [has207/xenia-edge #160](https://github.com/has207/xenia-edge/pull/160) | merged | `aa749b4be4f49ba9818eca1028ac77fb69f9d7b8` | G/H; not ported (RTV shader offset absent locally) | See scoped record below | RG-GDK-012 |
| [has207/xenia-edge #164](https://github.com/has207/xenia-edge/issues/164) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #194](https://github.com/has207/xenia-edge/pull/194) | closed unmerged | `9376b083bf8937a1bbc6fbff31cecc4c773a2c30` | B/H; useful research; rejected | See scoped record below; never assume absence | RG-GDK-010 |
| [has207/xenia-edge #198](https://github.com/has207/xenia-edge/issues/198) | closed | `not identified` | C/H; useful research | See scoped record below; never assume absence | RG-GDK-007 |
| [has207/xenia-edge #204](https://github.com/has207/xenia-edge/issues/204) | closed | `not identified` | B/H; useful research; vendor | See scoped record below; never assume absence | RG-GDK-006 |
| [has207/xenia-edge #207](https://github.com/has207/xenia-edge/pull/207) | closed unmerged | `551af133b2ffe8a4ab171dab9cb338b6a14d1665` | B/H; useful research | See scoped record below; never assume absence | RG-GDK-010 |
| [has207/xenia-edge #223](https://github.com/has207/xenia-edge/issues/223) | closed | `not identified` | F/H; regression-related; obsolete host path | See scoped record below; never assume absence | RG-GDK-011 |
| [has207/xenia-edge #233](https://github.com/has207/xenia-edge/issues/233) | closed | `not identified` | C/H; regression-related | See scoped record below; never assume absence | RG-GDK-015 |
| [has207/xenia-edge #235](https://github.com/has207/xenia-edge/issues/235) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #236](https://github.com/has207/xenia-edge/pull/236) | closed unmerged | `444746e4eb3d99a85f01137e0044641ec96701cb` | G/H; useful research; rejected | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #263](https://github.com/has207/xenia-edge/issues/263) | closed | `not identified` | B/H; regression-related | See scoped record below; never assume absence | RG-GDK-018 |
| [has207/xenia-edge #275](https://github.com/has207/xenia-edge/pull/275) | closed unmerged | `02d9ec4bddc6686f5a4c7f451faceee8f6dfe8ae` | G/H; useful research; game-specific; rejected | See scoped record below; never assume absence | RG-GDK-016 |

## Significant Canary and Edge candidates

### has207/xenia-edge #278 — Regression with depth_bias_shader_offset implementation

- Source: [https://github.com/has207/xenia-edge/issues/278](https://github.com/has207/xenia-edge/issues/278); created 2026-09-20T14:25:06Z; updated 2026-09-20T14:25:06Z; author `Slashic`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Lost Odyssey on RX 6600: depth_bias_shader_offset does not replace the old clamp successfully; do not enable globally.
- Classification: regression-related; game-specific; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-012 review (2026-09-25): open; the implicated shader offset is not in this repository.

### has207/xenia-edge #234 — Guest Scheduler regressions

- Source: [https://github.com/has207/xenia-edge/issues/234](https://github.com/has207/xenia-edge/issues/234); created 2026-08-21T06:35:02Z; updated 2026-09-12T12:54:02Z; author `has207`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Guest scheduler regressions and later recovery reports; runtime safepoints differ from static execution.
- Classification: regression-related; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-015 (2026-09-27): still open, updated 2026-09-27 with more titles hanging under `guest_scheduler=true`. **Not adopted**: ReXGlue has no guest scheduler. Every guest thread is a host thread; `rex::thread::Fiber` serves only guest-created fibers. The reports stay a reason not to import the scheduler. See [threading contracts](threading-contracts.md).

### has207/xenia-edge #251 — [Kernel] Park host-thread guest threads that keep delaying for zero time

- Source: [https://github.com/has207/xenia-edge/pull/251](https://github.com/has207/xenia-edge/pull/251); created 2026-08-27T04:05:32Z; updated 2026-09-21T13:06:18Z; author `xenios-jp`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `ecee74cb8d5300bb19ebbba3b53ec6ec2cf76044`.
- Scope / reason / applicability: Host-thread Sleep(0) parking changes guest timing; author did not test x64 or networking. Await evidence.
- Classification: experimental; relevant but pending upstream; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xthread.cc`; `src/xenia/kernel/xthread.h`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-015 (2026-09-27): still open, head `ecee74cb`. It targets host-thread mode, which is ReXGlue's only mode, so it applies in principle. **Kept as a watch**: it changes guest-visible timing (a `Sleep(0)` poll loop waits about 60 us), was measured on Halo Reach only, and not on x64 here. Quantum of Solace used about 0.9 to 1 core with and without precise timers (55 and 60 CPU-seconds over 60 s), with no zero-delay spin identified. Zero-time delays are untouched by RG-GDK-015. Adopt only with a local title that spins on zero delays and a before/after CPU and behaviour record.

### has207/xenia-edge #275 — [Kernel] Fix crash in UEFA Champions League 2006-2007 (title 45410811) from runaway audio buffer allocation

- Source: [https://github.com/has207/xenia-edge/pull/275](https://github.com/has207/xenia-edge/pull/275); created 2026-09-16T15:01:15Z; updated 2026-09-16T16:09:16Z; author `HandsleyD`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `02d9ec4bddc6686f5a4c7f451faceee8f6dfe8ae`.
- Scope / reason / applicability: Closed unmerged: title-specific 192KB allocation cap rejected. Maintainer identifies storage timing; examine 5e9eae601 instead.
- Classification: useful research; game-specific; rejected; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-016**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xboxkrnl/xboxkrnl_debug.cc`; `src/xenia/kernel/xboxkrnl/xboxkrnl_memory.cc`; `src/xenia/memory.cc`; `src/xenia/memory.h`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #236 — [APU] Emit silence instead of stalling on XMA hard decode errors (fixes LEGO LOTR intro freeze)

- Source: [https://github.com/has207/xenia-edge/pull/236](https://github.com/has207/xenia-edge/pull/236); created 2026-08-23T19:59:55Z; updated 2026-09-07T18:49:39Z; author `Forgottenshadow89`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `444746e4eb3d99a85f01137e0044641ec96701cb`.
- Scope / reason / applicability: Closed unmerged silence substitution, dependent on FFmpeg error propagation. Maintainer preferred actual decode fix.
- Classification: useful research; rejected; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/apu/xma_context_new.cc`; `src/xenia/apu/xma_context_new.h`; `src/xenia/apu/xma_decoder.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #235 — [Bug report] LEGO The Lord of the Rings: opening cutscene freezes — all XMA decoders stall on the same audio stream

- Source: [https://github.com/has207/xenia-edge/issues/235](https://github.com/has207/xenia-edge/issues/235); created 2026-08-23T15:39:18Z; updated 2026-08-23T16:24:53Z; author `Forgottenshadow89`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: LEGO LOTR intro stall motivates malformed-frame/cutscene progression tests; closure does not validate ReXGlue.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #141 — [XMA] The old & new version decoder have the BGM looping issue

- Source: [https://github.com/has207/xenia-edge/issues/141](https://github.com/has207/xenia-edge/issues/141); created 2026-04-16T07:53:55Z; updated 2026-09-06T08:13:57Z; author `lllljj1991`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Koei audio hitches described as looping; maintainer reports newer decoder fixed it. Preserve both symptom and uncertainty.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #263 — [Regression report] SmackDown! vs RAW 2007 audio stuttering frequently

- Source: [https://github.com/has207/xenia-edge/issues/263](https://github.com/has207/xenia-edge/issues/263); created 2026-08-31T18:09:05Z; updated 2026-09-12T13:52:38Z; author `RiasatSalminSami`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: SmackDown vs RAW 2007 reporter found improvements at 67e0804; CPU/storage changes confound attribution.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #233 — [Regression] Need for Speed Shift freezes when loading races since 34357e2

- Source: [https://github.com/has207/xenia-edge/issues/233](https://github.com/has207/xenia-edge/issues/233); created 2026-08-21T05:22:45Z; updated 2026-08-21T18:44:22Z; author `kllrvet`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: NFS Shift: ccd4443 good, 34357e2 bad, guest_scheduler=false workaround.
- Classification: regression-related; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-015 (2026-09-27): closed upstream; the freeze followed Edge scheduler commit `34357e2`, which ReXGlue does not have. Not applicable. NFS Shift is not available locally.

### has207/xenia-edge #160 — [GPU] FBO/ROV shader polygon offset for decal draws

- Source: [https://github.com/has207/xenia-edge/pull/160](https://github.com/has207/xenia-edge/pull/160); created 2026-05-11T07:09:35Z; updated 2026-05-29T04:19:53Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `aa749b4be4f49ba9818eca1028ac77fb69f9d7b8`.
- Scope / reason / applicability: Merged decal shader offset; comments report FH2 nearly invisible cars; #278 reports Lost Odyssey regression.
- Classification: candidate for later port; game-specific; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/pipeline_cache.cc`; `src/xenia/gpu/d3d12/pipeline_cache.h`; `src/xenia/gpu/draw_util.cc`; `src/xenia/gpu/draw_util.h`; `src/xenia/gpu/dxbc_shader_translator.cc`; `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_om.cc`; `src/xenia/gpu/spirv_shader_translator.cc`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_rb.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.h`; `src/xenia/ui/imgui_debug_dialog.cc`; `src/xenia/ui/imgui_debug_dialog.h`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-012 review (2026-09-25): not ported. Locally the RTV path applies polygon offset only as fixed-function `DepthBias` and the ROV path only in the shader (`edram_poly_offset_*`), selected by `edram_rov_used`, so there is no fixed-function plus shader double bias. The RTV shader offset from this PR is absent and stays out while has207/xenia-edge #278 (Lost Odyssey regression) is open.

### has207/xenia-edge #198 — depth_float24_convert_in_pixel_shader not working in d3d12

- Source: [https://github.com/has207/xenia-edge/issues/198](https://github.com/has207/xenia-edge/issues/198); created 2026-07-02T13:28:56Z; updated 2026-07-08T06:46:47Z; author `has207`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Closed report: float24 SPIR-V→DXIL translation failure in Tomb Raider Underworld, explicitly not a regression.
- Classification: useful research; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-007**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #204 — 45410811 - Flickering issue on my specific NVIDIA hardware

- Source: [https://github.com/has207/xenia-edge/issues/204](https://github.com/has207/xenia-edge/issues/204); created 2026-07-07T10:26:41Z; updated 2026-07-07T12:23:38Z; author `AhayriSG`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Closed for issue-scope reasons, not a demonstrated NVIDIA fix; follows game-compatibility #120.
- Classification: useful research; vendor; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-006**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #223 — [Linux/AMD] Readback Resolve broken after 9792c2b

- Source: [https://github.com/has207/xenia-edge/issues/223](https://github.com/has207/xenia-edge/issues/223); created 2026-08-04T05:17:08Z; updated 2026-09-08T22:37:53Z; author `StaydMcMuffin`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Linux/AMD Vulkan resolve report; informs readback tests, not a D3D12 driver fix.
- Classification: regression-related; obsolete host path; F/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-011**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #207 — [GPU] Add VIZ_QUERY predication

- Source: [https://github.com/has207/xenia-edge/pull/207](https://github.com/has207/xenia-edge/pull/207); created 2026-07-09T18:08:18Z; updated 2026-07-17T21:39:40Z; author `goldislead`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `551af133b2ffe8a4ab171dab9cb338b6a14d1665`.
- Scope / reason / applicability: VIZ proposal must be checked against current Canary #1111; do not infer settled predication from a closed PR.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-010**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/command_processor.cc`; `src/xenia/gpu/command_processor.h`; `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/d3d12_occlusion_query_pool.cc`; `src/xenia/gpu/d3d12/d3d12_occlusion_query_pool.h`; `src/xenia/gpu/d3d12/deferred_command_list.cc`; `src/xenia/gpu/d3d12/deferred_command_list.h`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/metal/metal_command_processor.cc`; `src/xenia/gpu/metal/metal_command_processor.h`; `src/xenia/gpu/null/null_command_processor.cc`; `src/xenia/gpu/null/null_command_processor.h`; `src/xenia/gpu/packet_disassembler.cc`; `src/xenia/gpu/packet_disassembler.h`; `src/xenia/gpu/pm4_command_processor_implement.h`; `src/xenia/gpu/shader_compiler_main.cc`; `src/xenia/gpu/spirv_fsi_system_constants.cc`; `src/xenia/gpu/spirv_fsi_system_constants.h`; `src/xenia/gpu/spirv_shader_translator.cc`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_rb.cc`; `src/xenia/gpu/vulkan/deferred_command_buffer.cc`; `src/xenia/gpu/vulkan/deferred_command_buffer.h`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_occlusion_query_pool.cc`; `src/xenia/gpu/vulkan/vulkan_occlusion_query_pool.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/ui/imgui_performance_dialog.cc`; `src/xenia/ui/imgui_performance_dialog.h`; `src/xenia/ui/vulkan/functions/device_ext_conditional_rendering.inc`; `src/xenia/ui/vulkan/vulkan_device.cc`; `src/xenia/ui/vulkan/vulkan_device.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #268 — Overflowed stackpoints! Crash

- Source: [https://github.com/has207/xenia-edge/issues/268](https://github.com/has207/xenia-edge/issues/268); created 2026-09-10T01:34:03Z; updated 2026-09-15T14:38:37Z; author `farenthy`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Stackpoint overflow is a JIT execution symptom; retain guest exception test idea, do not import code cache.
- Classification: useful research; emulator-specific; D applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-015 (2026-09-27): "Overflowed stackpoints" is Xenia's JIT stack-point tracking, which ReXGlue does not have (static recompilation, ADR-004). Not applicable; no stack-point or safe-point machinery is imported.

### has207/xenia-edge #38 — Maybe implement XamSwapCancel

- Source: [https://github.com/has207/xenia-edge/issues/38](https://github.com/has207/xenia-edge/issues/38); created 2025-12-01T13:51:23Z; updated 2025-12-02T00:29:27Z; author `has207`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: XamSwapCancel matters only if a static title exercises disc transitions; include module/media lifecycle research.
- Classification: candidate for later port; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1111 — [GPU] VIZ_QUERY predication

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1111](https://github.com/xenia-canary/xenia-canary/pull/1111); created 2026-07-22T22:28:10Z; updated 2026-09-22T05:27:03Z; author `goldislead`.
- Upstream status: **pending upstream** (still open at the RG-GDK-010 review, 2026-09-24). PR head (not adopted) commit `78e06cafaa6429e5464786baa9e1edf252bbe582`.
- Scope / reason / applicability: WIP VIZ predication changes query lifecycle, host predicates and ROV counters; local visibility is still faked (every VIZ query reports visible).
- Classification: relevant but pending upstream; experimental; C/H applicability. **Not adopted.** RG-GDK-010 deferred VIZ to [#65](https://github.com/furqanagwan/rexglue-sdk/issues/65), gated on #1111 settling.
- Update 2026-10-03: merged into Canary as `692cd59cf` (2026-09-29), which Edge also took. Ported in RG-GDK-010b; see the section at the end of this file.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1077 — [GPU] Clamp depth to valid value if Inf is provided

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1077](https://github.com/xenia-canary/xenia-canary/pull/1077); created 2026-07-05T20:37:26Z; updated 2026-09-21T20:50:57Z; author `Gliniak`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `dcd2fff24243b4d2d67c2d08a10d235f04f0de80`.
- Scope / reason / applicability: Depth Inf clamp author explicitly requests correctness research; do not adopt globally.
- Classification: experimental; game-specific; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/dxbc_shader_translator_fetch.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-012 review (2026-09-25): still open upstream; not adopted. No depth clamp for Inf is added.

### xenia-canary/xenia-canary #1182 — [Memory] Fix large-alignment physical allocs through offset-translated heaps

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1182](https://github.com/xenia-canary/xenia-canary/pull/1182); created 2026-08-27T20:26:17Z; updated 2026-08-27T20:27:26Z; author `shempman828`.
- Upstream status: **pending upstream** (still open 2026-09-24). PR head (not adopted) commit `fa6cdaae0f58e9161e5e41ea3683f837b3b112c7`.
- Scope / reason / applicability: makes vE0000000 virtual addresses aligned (an `alignment_phase` for the parent search) instead of physical ones; title 4D5307F1 failed a 0x280000-byte 32 KB-aligned request in Canary.
- Classification: relevant but pending upstream; B/H applicability. **Watch, not adopted (RG-GDK-004, 2026-09-24).** The 4D5307F1 request succeeds here with a 32 KB-aligned physical address, because there is no host alignment check to fail. Aligning the virtual address instead would move the physical address off the requested alignment, a global change with no title evidence here.
- Tests: `Physical heap vE0000000 aligns the physical address` records the current semantics (physical address aligned, virtual address 0x1000 below it) and the 4D5307F1 request size.

### xenia-canary/xenia-canary #1225 — [Kernel] Take back the signature a dying object left in guest memory

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1225](https://github.com/xenia-canary/xenia-canary/pull/1225); created 2026-09-10T15:48:02Z; updated 2026-09-12T21:42:06Z; author `peerloomllc`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `fe960bf66f98204940a7464ed35b50ef5b7b4cdc`.
- Scope / reason / applicability: Guest object signature lifetime; maintainer requests root-cause research, Guitar Hero 5 DLC reproducer.
- Classification: relevant but pending upstream; blocked upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-014**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xobject.cc`; `src/xenia/kernel/xobject.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-014 (2026-09-27): **read side only**. A synthetic reproducer (`kernel_tests [object_header]`, "A signature left by a dead object ...") shows the mechanism locally: an object that `GetNativeObject` created over guest memory dies on `ObDereferenceObject`, the table reissues its handle to the next object, and the stale signature resolved to that unrelated object. Lookups now use a stashed handle only when its object's `guest_object()` is this memory, and otherwise create the object afresh; removing that check fails the test. Not adopted: clearing the signature when the object dies (it writes guest memory the guest may inspect) and any change to whether that object should die, which is the ownership question the maintainer asked to research. Guitar Hero 5 is not available here.

### xenia-canary/xenia-canary #1226 — [VFS] Bounds check the STFS reader

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1226](https://github.com/xenia-canary/xenia-canary/pull/1226); created 2026-09-10T15:48:14Z; updated 2026-09-12T21:51:16Z; author `peerloomllc`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `c43ea0f9c3e3f3cac600a57d9f464a38c945d808`.
- Scope / reason / applicability: Truncated STFS package bounds; local parser exists but package-manager APIs differ.
- Classification: pending upstream (open, reviewed with the maintainer, final head `c43ea0f9c`); B. **Adapted 2026-09-26 (RG-GDK-017 part 2)** from its final state: the mount-time `content_size` check (a package whose metadata describes more data than the file holds is refused and named; zero means unknown) and a block chain stopping where `GetBlockHash` finds no hash table. ReXGlue's reader uses `fread`, not a mapping, so the crash shape differs: its callers dereferenced `GetBlockHash`'s null. Audited beyond the PR: parent `directory_index` bounds and kind, name length clamped to the 40-byte field, SVOD node cycles and depth, SVOD data-file index, reads that stop at a short `fread`, `ReadMagic` on files shorter than 4 bytes, and `assert_always` on malformed chains replaced with the existing warnings (input, not invariants). Tests: `unit_tests [stfs],[svod]` on synthetic packages; 7 of 13 fail on the previous reader. See `docs/content-persistence.md`.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xam/xcontent/xcontent_package_container.cc`; `src/xenia/vfs/devices/xcontent_devices/stfs_container_device.cc`; `src/xenia/vfs/devices/xcontent_devices/stfs_container_device.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1214 — [APU] Lock order inversion between UnregisterClient and the audio worker

- Source: [https://github.com/xenia-canary/xenia-canary/issues/1214](https://github.com/xenia-canary/xenia-canary/issues/1214); created 2026-09-05T18:49:02Z; updated 2026-09-05T18:49:02Z; author `peerloomllc`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Lock inversion report points to Edge 8aa50e0e0. Local locking differs; audit callback lifetime instead of copying locks.
- Classification: useful research; regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1025 — [Kernel] Fixed use-after-free on title threads termination

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1025](https://github.com/xenia-canary/xenia-canary/pull/1025); created 2026-05-24T22:48:02Z; updated 2026-08-25T20:01:45Z; author `AdrianCassar`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `abcf2ff1bea480cb6c4cdcafd2c09350826e01d3`.
- Scope / reason / applicability: Thread teardown/XEX swap UAF; static module lifecycle needs independent ownership design.
- Classification: relevant but pending upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/kernel_state.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-015 (2026-09-27): still open, head `abcf2ff1`. It fixes iterator use after `TerminateTitle` drops the global lock mid-loop, and relies on `StepToGuestSafePoint`, which is JIT machinery. **Not needed here**: ReXGlue's `TerminateTitle` retains every target thread (`object_ref`), never drops the lock while iterating, and stops threads cooperatively at kernel wait points instead of suspending them. `kernel_tests [termination]` runs 10 rounds of 16 blocked guest threads under contention, in both timer modes, with a watchdog.

### xenia-canary/xenia-canary #748 — Fix audio quality: Use proper rounding in float to int16 conversion

- Source: [https://github.com/xenia-canary/xenia-canary/pull/748](https://github.com/xenia-canary/xenia-canary/pull/748); created 2025-10-19T11:50:50Z; updated 2026-08-25T20:01:45Z; author `tygyh`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `a7f6514e699018674f5e3f56b437e4d9c58e7641`.
- Scope / reason / applicability: Scalar float→int16 rounding PR may miss SIMD path according to review; not a general audio-quality cure.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/apu/xma_context.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #844 — [Config] Do not overwrite base config if game specific config is loaded

- Source: [https://github.com/xenia-canary/xenia-canary/pull/844](https://github.com/xenia-canary/xenia-canary/pull/844); created 2026-01-10T19:26:33Z; updated 2026-08-25T20:01:45Z; author `Gliniak`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `16c13ed6ca60c20e4611e804216f78f5261c99ba`.
- Scope / reason / applicability: Prevent title configuration contaminating global config; profile design should test transition and persistence.
- Classification: relevant but pending upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-013**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/config.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-013 (2026-09-27): still open (head `16c13ed6`, updated 2026-08-25). Not ported: ReXGlue has no per-title config files. ADR-009 addresses the cause instead: profile values are their own cvar source below the user config, and `SaveConfig` may write only `config` and `runtime` values. The same leak exists locally today for command-line and environment values; the rule fixes both.

### xenia-canary/xenia-canary #608 — native stencil value output doesn't work on non-Arc Intel gpus like iris xe

- Source: [https://github.com/xenia-canary/xenia-canary/issues/608](https://github.com/xenia-canary/xenia-canary/issues/608); created 2025-05-01T05:44:27Z; updated 2025-05-01T05:44:27Z; author `D2firegit`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Intel non-Arc RTV stencil artifacts remain an open report; native stencil toggle is not proof of support.
- Classification: regression-related; vendor; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-006**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1093 — 5451087D - UFC Undisputed 3: fighter meshes render as black collapsed triangles (memexport stream fails when guest physical heap is near-full)

- Source: [https://github.com/xenia-canary/xenia-canary/issues/1093](https://github.com/xenia-canary/xenia-canary/issues/1093); created 2026-07-15T15:43:59Z; updated 2026-07-15T22:20:11Z; author `cozycaston`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: UFC3 memexport/heap issue, Windows RTX 4070 Ti SUPER; comments distinguish stock and created fighters and Vulkan/D3D12.
- Classification: regression-related; game-specific; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-011**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-011 review (2026-09-24): still open upstream with no merged fix in Canary or Edge for D3D12. The proposed clamp of each memexport stream to its committed allocation (issue comment) is not adopted: unmerged, and UFC Undisputed 3 isn't available to validate it. Locally a failed memexport `RequestRange` still drops the draw with an error rather than being widened or ignored. Near-full-heap memexport fixtures are covered by `gpu_tests [memexport]` (RG-GDK-011 part 2): a stream running 1 MB past its allocation keeps the head written and the command processor running.

### xenia-canary/xenia-canary #1127 — [GPU/DXBC] Fix signed round bias breaking memexport

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1127](https://github.com/xenia-canary/xenia-canary/pull/1127); created 2026-08-02T10:47:26Z; updated 2026-08-02T16:05:24Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `da47dfaacfae1134b238af9083a7f3d413c6cbbe`.
- Scope / reason / applicability: Signed memexport rounding destination error confirmed in both local sites.
- Classification: candidate for later port; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-005**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/dxbc_shader_translator_memexport.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1215 — [Memory] Round the AllocRange ceiling to the page, not to the alignment

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1215](https://github.com/xenia-canary/xenia-canary/pull/1215); created 2026-09-06T01:19:39Z; updated 2026-09-07T18:30:42Z; author `peerloomllc`; ported from xenia-edge `1ad151d1`.
- Upstream status: **merged upstream**. Merge commit `87c24112706d95f15f83dfec58e93923bd7ffa07`.
- Scope / reason / applicability: `BaseHeap::AllocRange` rounded `high_address` up to the alignment, so an allocation could end above the caller's ceiling, and a ceiling near `UINT32_MAX` wrapped to zero.
- Classification: correctness; B/H applicability. **Adopted 2026-09-24 in RG-GDK-004 (adapted).**
- Adaptation: the ceiling is no longer aligned. Canary rounds `high_page_number` up to the page because its free-block search treats it as exclusive (xenia-edge `4fcb8e449`); this repository still has the older search, which treats it as the last usable page, so the local port uses the last page ending at or below `high_address` instead. The upstream formula would allow one page above the ceiling here. For windows ending one byte below an alignment boundary, allocations whose size is a multiple of the alignment land where they did before (the Far Cry 3/4 and Watch Dogs layout concern in `4fcb8e449`).
- Tests: `tests/unit/memory/heap_allocation_test.cpp` AllocRange window cases (unaligned ceiling, inclusive page ceiling, exact fit top-down/bottom-up, `UINT32_MAX` ceiling, windows too small, Canary's physical-heap case, vE0000000 4K/32K/64K). Six of the seven cases fail on the previous code; the exact-fit case passes on both.
- Far Cry 3 or an equivalent physical-heap title was not run (no title content).

### xenia-canary/xenia-canary #1202 — [Memory] Check the host alignment PhysicalHeap can actually satisfy

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1202](https://github.com/xenia-canary/xenia-canary/pull/1202); created 2026-09-02T23:06:06Z; updated 2026-09-04T04:39:16Z; author `peerloomllc`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `1cc288bc71e625bd9272dafc2f1fb01259a62eb2`.
- Scope / reason / applicability: Closed unmerged: maintainer warns the alignment proposal changes poorly understood/unreachable checks, distinguishes AllocFixed, and names Far Cry 3 as a regression control. Keep separate from merged #1215 ceiling fix.
- Classification: useful research; rejected; B/H applicability. **Not adopted (RG-GDK-004, 2026-09-24): not applicable.** `PhysicalHeap::Alloc`/`AllocRange` here have no host alignment check at all; the caller's alignment is applied to the physical address by the parent heap, which is the semantics #1202 argues for. Covered by `Physical heap vE0000000 aligns the physical address`.

### xenia-canary/xenia-canary #1243 — [GPU] Misc guest texture layout edge cases

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1243](https://github.com/xenia-canary/xenia-canary/pull/1243); created 2026-09-22T07:40:19Z; updated 2026-09-22T17:46:24Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `c9c8e483b13249ba5f0060fc4b36071da6d67261`.
- Scope / reason / applicability: Array/volume mip layout, 96bpp pitch and 3D bounds differ locally; CPU fixtures plus texture readback.
- Classification: correctness; A/B applicability. **Adopted 2026-09-24 in RG-GDK-008.**
- Adaptation and validation owner: **RG-GDK-008**.
- Source files: `src/xenia/gpu/texture_util.cc`; `src/xenia/gpu/texture_util.h`.
- Commits: `94cb5e349f` array/volume mip layout (Edge `bc6b7a020`), `21543817f2` linear 96bpp mip row pitch (Edge `19da17403`), `19f5a08556` tiled 3D address bounds (not in Edge at `94de4f676`; taken from the PR head).
- Adaptation: applied unchanged to `src/graphics/pipeline/texture/util.cpp` and `include/rex/graphics/pipeline/texture/util.h`; no texture-cache lifetime change.
- Tests: `tests/unit/graphics/texture_layout_test.cpp` (`[texture_layout]`): hand-derived D3D offsets for 2D array, 3D and packed 3D mips and linear row pitches, plus a per-texel `GetTiledOffset2D/3D` oracle that the 2D/3D bounds must contain (and match exactly for whole 32x32x4 tiles). Every case except the whole-tile tightness check fails on the pre-port code.
- Regression evidence: none known upstream at adoption.

### xenia-canary/xenia-canary a635ac64f — [GPU] Scaled resolve readback through downscale CS

- Source: xenia-canary commit `a635ac64f5ca37c0b789e8b4166b53dc673b213f` (2026-07-06, `goldislead`), present in Edge at `94de4f676`.
- Classification: correctness; A/B applicability. **Partially adopted 2026-09-24 for #58.**
- Adopted: texel size from the resolve's normalized `copy_dest_info` (`draw_util::GetResolveDownscalePixelSizeLog2`, `Resolve(..., copy_dest_info_out)`), the source window at the written extent's scaled address instead of the scaled range start, and skipping 128bpp, unaligned and out-of-range extents.
- Not adopted: the shader's group layout. Canary reads the scaled layout introduced by `0f23f0568` (2026-01-13); this repository's resolve shaders are Canary `0b2ffa314` (2025-08-20; see the shader sources record below), which writes Nx1 units. `resolve_downscale.cs.hlsl` reverses the Nx1 layout instead. Reading the group layout passes at 2x but misplaces texels at 3x.
- Local changes: the extent is limited in dwords rather than truncated to whole 32x32 tiles, so a resolve ending inside a tile reads back completely. The HLSL shifts before masking because fxc 10.1 compiles `(a & mask(n + s)) >> s` into a `ubfe` of width `n + s`, which shifted 16bpp and 32bpp texels by one unit.
- Tests: `Resolve readback keeps texel positions` and `gpu.resolve_readback_scaled_3x2`; see `docs/regression-strategy.md`.
- Follow-up: syncing the resolve and texture-load shaders to Canary's current layout needs the downscale shader to move with them.

### xenia-canary/xenia-canary 0b2ffa314 — precompiled shader sources

- Source: xenia-canary commit `0b2ffa3143` (2025-08-20, "[GPU] Change texture load cbuffer to push constants"), `src/xenia/gpu/shaders` and `src/xenia/ui/shaders/xesl.xesli`, taken from the Edge clone.
- Classification: provenance; A applicability. **Adopted 2026-09-24 for RG-GDK-009.**
- Evidence: FXC 10.1 (Windows SDK 10.0.26100.0) with Canary's `xenia-build buildshaders` arguments rebuilds all 107 checked-in `src/graphics/shaders/bytecode/d3d12_5_1` headers that have sources byte for byte. The earlier `04d5c40d0` attribution matched the resolve shaders only; its texture-load shaders differ.
- Vendored: every `src/xenia/gpu/shaders` XeSL/HLSL source except `fxaa.cs.hlsl`, `fxaa_extreme.cs.hlsl` and `fxaa.hlsli`, which need `third_party/fxaa/FXAA3_11.h`; their bytecode stays as checked in. `src/ui/shaders/bytecode` sources are not vendored.
- Tooling: `scripts/build_shaders.py` builds the bytecode with the same arguments and clang-formats it; `--check` compares with the checked-in headers and runs as CTest `shaders.bytecode_reproducible` on Windows. `resolve_downscale_cs.h` is rebuilt with these arguments instead of `/O3` and the strip flags.
- Not adopted: Canary's later shader changes, including the `0f23f0568` scaled group layout; later ports edit these sources and rebuild.

### xenia-canary/xenia-canary #1240 — [GPU] Fix tiled resolve offsets below 32bpp

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1240](https://github.com/xenia-canary/xenia-canary/pull/1240); created 2026-09-21T04:23:51Z; updated 2026-09-21T04:50:35Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `89609297c7ae25dc5ba404c6a977260b806bb725`.
- Scope / reason / applicability: Sub-32bpp tiled resolve macro phase, title 534307D5 water; general addressing candidate.
- Classification: correctness; A/B applicability. **Adopted 2026-09-24 in RG-GDK-008.**
- Adaptation and validation owner: **RG-GDK-008**.
- Source files: `src/xenia/gpu/draw_util.cc` (Edge `89609297c`).
- Adaptation: applied unchanged to `GetResolveInfo` in `src/graphics/util/draw.cpp`.
- Tests: GPU fixture `Sub-32bpp resolve keeps the macro tile phase of the base` resolves 32x32 rectangles to k_8 (phase 0, 1, 3) and k_5_6_5 (phase 0, 1) bases and checks every byte of a 16 KB allocation against `GetTiledOffset2D`. Phase 1/3 cases fail on the pre-port code (every texel misplaced).
- Title 534307D5 (Golden Axe: Beast Rider) is not available locally; no title result is claimed.
- Regression evidence: none known upstream at adoption.

### xenia-canary/xenia-canary #1238 — [GPU] Fix 2x & 4x host depth copy sample layout

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1238](https://github.com/xenia-canary/xenia-canary/pull/1238); created 2026-09-19T19:47:34Z; updated 2026-09-20T16:40:37Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `0bd090dbe979bc64959efe519aa320db8e3eb467`.
- Scope / reason / applicability: 2x/4x depth copy sample layout repairs an AMD regression after canonical EDRAM; mandatory companion for #1163.
- Classification: regression fix; B/H applicability. **Adopted 2026-09-24 in RG-GDK-009.**
- Adaptation: `host_depth_store_2xmsaa.cs.xesl` and `host_depth_store_4xmsaa.cs.xesl` pass sample coordinates through the 1x path, in `XeEdramOffsetInts` units. The 4x change produces the same stores here: the shader addresses the buffer in 16-byte units, so the thread parity bit Canary's byte-addressed version added was already dropped.
- Tests: `MSAA float24 depth keeps host precision through an alias` (2x fails in 486 of 512 pixels without the 2x change).
- AMD not available locally; the AMD regression itself is not reproduced (non-blocking, ADR-007).

### xenia-canary/xenia-canary #1163 — [GPU] One canonical EDRAM layout

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1163](https://github.com/xenia-canary/xenia-canary/pull/1163); created 2026-08-20T03:08:53Z; updated 2026-08-26T17:05:59Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `437a7280cf95310d518a2f68087aab61403956ac`.
- Scope / reason / applicability: one canonical sample layout for 1x/2x/4x views of the same EDRAM (4x4 sample blocks, sample bit 0 horizontal, 2x sample 0 top); not independently safe, adopted together with #1238 and #1222.
- Classification: correctness; C/H applicability. **Adopted 2026-09-24 in RG-GDK-009.**
- Adaptation: shaders ported onto the vendored `0b2ffa314` sources (int-addressed EDRAM buffers instead of Canary's later byte addressing): `edram.xesli` remaps pixels and samples to canonical coordinates at guest pixel granularity; `resolve.xesli` and the full resolves load each sample by address; the fast resolves keep a vectorized path for 1x sources and load per pixel for MSAA, so they bind the EDRAM as a raw buffer (`draw_util::resolve_copy_shader_info`). The D3D12 transfer and dump shader generators and the ROV output (`dxbc_translator_om.cpp`) come from Canary's diff re-based on clang-formatted sources; Canary's per-target native scale (`source_scale_native`, `native_layout`, from `74db632ab`) doesn't exist here, so the layout scale is always the draw resolution scale. Adds `dxbc::Src::kXYXY`. Vulkan/SPIR-V parts not applicable (removed backend).
- Tests: `[edram]` GPU fixtures (1x/2x/4x re-aliasing in both directions, 64bpp as 32bpp at 1x/4x); all `[gpu]` fixtures at native, 3x2 and 2x2 scale on host RT and ROV. See `docs/regression-strategy.md`.
- Not covered: MSAA resolve averaging order (`k01` now averages horizontal samples), PWL gamma blend, titles 5841125E/4D5307F1.

### xenia-canary/xenia-canary #1222 — [GPU] EDRAM bits respected for color/depth aliases

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1222](https://github.com/xenia-canary/xenia-canary/pull/1222); created 2026-09-09T10:26:39Z; updated 2026-09-10T05:31:18Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `9da693480d0995326b81c3a14f8b1a4c226066eb`.
- Scope / reason / applicability: keep depth/stencil enabled when an aliased color target writes only bits the tests don't use; title 4D530A26 (clouds and sprites through geometry), possibly other UE3.5 titles.
- Classification: compatibility/correctness; B/H applicability. **Adopted 2026-09-24 in RG-GDK-009.**
- Adaptation: `RenderTargetCache::ColorOverlapsDepthStencil`, per-range `depth_bits_target`, `IsHostDepthCurrent` and read-only aliased depth for host render targets behind the new `aliased_depth_read_only` cvar (default true, as upstream); D3D12 ROV check in `UpdateSystemConstantValues_Impl`. Canary's `scale_native` key field is not present here and is omitted.
- Tests: `Color aliasing depth keeps a read-only depth test` (without the change 640 of 640 depth-failing pixels are written, host RT and ROV).
- Title 4D530A26 is not available locally; no title result is claimed.

### xenia-canary/xenia-canary #1218 — [GPU] ZPD as a running sample counter; QueryBatch support

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1218](https://github.com/xenia-canary/xenia-canary/pull/1218); created 2026-09-07T04:36:41Z; merged 2026-09-20T16:14:05Z; author `goldislead`. Head `be167dd9358f120dfb5562ba93c440a7b5bf9e2a`, merge commit `3d233a5b2e94b940825847b70c788951e364bb33` (parent `aee0871dd`). Reviewed 2026-09-24.
- Upstream status: **merged upstream**. No follow-up regression reports found at review time.
- Scope: each EVENT_WRITE_ZPD ends the interval since the previous one and queues a snapshot of a free-running counter (conventional BEGIN/END and QueryBatch alike); reports retire in stream order; host query segments split at submissions and draw-scale changes; slots are recycled with generation checks; fast modes write a visible-biased guess and correct it on retire; strict waits only for reports holding the D3D sentinel. Also binds an empty PS to RTV draws without a PS or depth/stencil writes, which D3D otherwise drops (4541096E, 5553083B).
- Classification: correctness; C/H applicability. **Adopted in RG-GDK-010 (D3D12 native-query subset).**
- Adaptation: the local path was an older sentinel-inferring BEGIN/END pair that ended the submission at END and fell back to 1000 fake samples whenever the fence had not passed, which on a real GPU was nearly always. Replaced by `XenosZPDReport` (`include/rex/graphics/xenos_zpd_report.h`), the shared state machine in `CommandProcessor` and `D3D12ZPDQueryPool` (`src/graphics/d3d12/zpd_query_pool.cpp`). `PipelineCache::AwaitPipelineCompletion` ported for strict waits behind async pipeline creation. New `occlusion_query` cvar (`fake`/`fast`/`fast-alt`/`strict`, default `fast`); `occlusion_query_enable=false` still forces fake and `query_occlusion_fake_sample_count` (1000) is the per-interval fake delta instead of Canary's 80–100 walk. No draw-resolution-scale threshold exists locally, so segments use the texture cache scale.
- ROV counters, **adopted 2026-09-28 (RG-GDK-010a)** from the same commit:
  - The ROV pixel shaders add their surviving coverage to the open query's four-lane counter slot with `atomic_iadd` (`ROV_AddMSAASamplesToZPD`, `zpd_counter_index` in the former padding system constant).
  - With `occlusion_query_full_counters`, they also add depth-failed (bits 12:15) and stencil-failed (bits 16:19) samples. StencilFail takes precedence, and failures are counted before an early-depth quad discard.
  - `D3D12ZPDQueryPool` gained the counter buffer, clear and readback. It clears by copying from a zeroed slot, because the deferred command list has no `WriteBufferImmediate`.
  - The counter UAV is bound after the EDRAM UAV in both bindful tables, and at `SystemBindlessView::kZpdCounterRawUAV` in bindless mode.
  - Supporting changes: new `countbits` and `atomic_iadd` DXBC opcodes, and pipeline storage version `0x20260928`.
  - Tests: `gpu_tests [zpd][rov]`.
- RTV hybrid Total, **adopted 2026-09-29 (RG-GDK-010a, [#64](https://github.com/furqanagwan/rexglue-sdk/issues/64))** from the same commit:
  - With host render targets and `occlusion_query_full_counters`, draws with a depth or stencil test and no depth write, inside a query, get the `zpd_total` pixel shader modification (early depth hint dropped). The shader adds the coverage entering the depth / stencil test, after alpha test and alpha to coverage, to the slot's Total (`RTV_AddMSAASamplesToZPDTotal`; with sample-rate shading only the first covered sample adds it).
  - Draws without a guest pixel shader use counting depth-only shaders (plain, float24 truncating and rounding), selected by a new `PipelineDescription::zpd_total` bit (pipeline version `0x20260929`).
  - Query segments split when Total counting changes (`UpdateZPDSegment(scale, count_total)`). A hybrid query clears its slot, records a D3D12 query for ZPass, resolves both, and reports `FromNativeQueryAndTotal` (ZFail = Total - ZPass).
  - While the counting pipeline compiles asynchronously, the draw falls back to the plain pipeline rather than being skipped.
  - Adaptation: rexglue clears slots by copy, so Canary's `CommandList2` condition is dropped. The counter UAV is added to RTV root signatures and bindful tables only when hybrid queries are supported, so default layouts are unchanged. ROV pipeline storage gets Canary's `-fc` suffix with full counters.
  - Fix found while porting: the pixel shader input signature redeclared `sample_index_position` inside its `if`, so `SV_SampleIndex` never got its semantic name (memexport with sample-rate shading). Canary does not have this.
  - Tests: `gpu_tests [zpd][hybrid]`.
- Vulkan/SPIR-V parts are not applicable. `occlusion_query_saturation` (Edge #143) is not revived.
- Validation: `unit_tests [zpd]` (lane split, 32-bit wrap, scale normalization, write order) and `gpu_tests [zpd]` — conventional BEGIN/END, QueryBatch with empty intervals, depth-rejected samples, segments across submissions, reused report memory with recycled host slots (96 queries), a PS-less no-write draw, 4x MSAA, fast-mode guess then correction, and fake mode — on NVIDIA (0x10DE, driver 32.0.16.1714) at 1x and 3x2 draw resolution scale and on WARP. The PS-less case reads 0 samples on both NVIDIA and WARP without the empty-PS binding. MSAA counts host samples as Canary does; console behaviour is not verified. AMD/Intel and Crackdown 2 (no title content) are not run; the ROV path is covered on NVIDIA since RG-GDK-010a.
- Source files: `src/xenia/gpu/command_processor.cc`; `src/xenia/gpu/command_processor.h`; `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.cc`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.h`; `src/xenia/gpu/d3d12/pipeline_cache.cc`; `src/xenia/gpu/d3d12/pipeline_cache.h`; `src/xenia/gpu/pm4_command_processor_implement.h`; `src/xenia/gpu/xenos_zpd_report.h` (ported); DXBC translator, `gpu_flags` full-counter cvar and all Vulkan/SPIR-V files (not ported).
- Regression evidence: none known upstream at review time; local fixtures above.

### xenia-canary/xenia-canary #1016 — [GPU] Implement ZPD occlusion queries

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1016](https://github.com/xenia-canary/xenia-canary/pull/1016); created 2026-05-16T14:59:45Z; updated 2026-05-30T17:26:26Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `fbd620c22b44638b66a70bba80d6f30d55a10924`.
- Scope / reason / applicability: Original ZPD implementation is historical context, superseded by running counters.
- Classification: useful research; relevant and merged upstream; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-010**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/command_processor.cc`; `src/xenia/gpu/command_processor.h`; `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.cc`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.h`; `src/xenia/gpu/d3d12/deferred_command_list.cc`; `src/xenia/gpu/d3d12/deferred_command_list.h`; `src/xenia/gpu/d3d12/pipeline_cache.cc`; `src/xenia/gpu/d3d12/pipeline_cache.h`; `src/xenia/gpu/dxbc.h`; `src/xenia/gpu/dxbc_shader_translator.cc`; `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_om.cc`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/pm4_command_processor_implement.h`; `src/xenia/gpu/spirv_shader_translator.cc`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_rb.cc`; `src/xenia/gpu/vulkan/deferred_command_buffer.cc`; `src/xenia/gpu/vulkan/deferred_command_buffer.h`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.h`; `src/xenia/gpu/vulkan/vulkan_zpd_query_pool.cc`; `src/xenia/gpu/vulkan/vulkan_zpd_query_pool.h`; `src/xenia/gpu/xenos_zpd_report.h`; `src/xenia/ui/vulkan/functions/device_1_0.inc`; `src/xenia/ui/vulkan/functions/device_1_2_ext_host_query_reset.inc`; `src/xenia/ui/vulkan/vulkan_device.cc`; `src/xenia/ui/vulkan/vulkan_device.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1058 — [GPU] Rolls back ZPD cache erasure on guest BEGIN. Only affects kFast.

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1058](https://github.com/xenia-canary/xenia-canary/pull/1058); created 2026-06-19T02:37:42Z; updated 2026-06-19T06:02:34Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `d55670e40b1016cc36cca5111c821b3e7c9a85b8`.
- Scope / reason / applicability: Fast ZPD cache-erasure rollback shows why original query port must include later corrections.
- Classification: regression-related; B/H applicability. Its behaviour is kept by the RG-GDK-010 port of #1218: the fast-mode per-address delta cache is never erased on a guest BEGIN (there is no BEGIN state any more), only capped at 1024 entries.
- Source files: `src/xenia/gpu/command_processor.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1190 — [GPU] Replace AC6 ground hack with scalar approximation rounding

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1190](https://github.com/xenia-canary/xenia-canary/pull/1190); created 2026-08-29T06:31:47Z; updated 2026-08-31T20:07:05Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `3a44f20c7bc66db1da583e8a6f0ab740e31908e9`.
- Scope / reason / applicability: AC6 workaround replaced with scalar approximation; numerical semantics before a title gate.
- Classification: candidate for later port; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_alu.cc`; `src/xenia/gpu/dxbc_shader_translator_fetch.cc`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/shader_interpreter.cc`; `src/xenia/gpu/shader_interpreter.h`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_alu.cc`; `src/xenia/gpu/spirv_shader_translator_fetch.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-012 decision (2026-09-25): **adopted opt-in**, `gpu_scalar_approximation_rounding` (default off, restart). The commit itself says the console precision and halfway behavior are unconfirmed, and #12 forbids game-motivated math defaults without an independent oracle. Ported for DXBC (`ReduceFloatPrecision`, DIV/SQRT reciprocal instead of RCP/RSQ when enabled) and the CPU shader interpreter (`include/rex/graphics/pipeline/shader/float_precision.h`); the local translator never had `ac6_ground_fix`, so nothing is removed. SPIR-V parts are not applicable. Tests: `unit_tests [float_precision]` (rounding, halfway, carry, signed zero, Inf/NaN, FLT_MAX not rounded to infinity) and `gpu_tests [alu]`, which runs EXP/LOG/LOGC/RCP/RCPC/RCPF/RSQ/RSQC/RSQF/SQRT on 27 inputs through translated vertex shaders (memexport readback) with the option off and on, checking the documented special cases bit-exactly and finite results against double precision within 2^-20; passes on NVIDIA and WARP both ways. 4E4D07D1 (AC6) is not available, so the title benefit is not reproduced.

### xenia-canary/xenia-canary #1031 — [GPU] Remove ac6_ground_fix - nudge rcp up slightly instead

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1031](https://github.com/xenia-canary/xenia-canary/pull/1031); created 2026-05-26T22:42:05Z; updated 2026-09-01T09:10:47Z; author `oreyg`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `93e3fa59b040f124f0343a80b90f2d0b53b125fc`.
- Scope / reason / applicability: Earlier AC6 workaround-removal approach; status must not be conflated with merged #1190.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_alu.cc`; `src/xenia/gpu/dxbc_shader_translator_fetch.cc`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_alu.cc`; `src/xenia/gpu/spirv_shader_translator_fetch.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1180 — [Memory/GPU/Kernel] Initial XPS Support

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1180](https://github.com/xenia-canary/xenia-canary/pull/1180); created 2026-08-27T07:20:41Z; updated 2026-08-30T07:05:50Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `22708301ba76d10aae6f7d7caac8b1cac9e4a8e6`.
- Scope / reason / applicability: XPS memory/GPU/kernel support crosses ABI boundaries; no bulk subsystem replacement.
- Classification: useful research; redesign; C/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-004**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/dxbc_shader_translator_fetch.cc`; `src/xenia/gpu/pm4_command_processor_implement.h`; `src/xenia/gpu/shader_interpreter.cc`; `src/xenia/gpu/spirv_shader_translator_fetch.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/kernel/xboxkrnl/xboxkrnl_memory.cc`; `src/xenia/memory.cc`; `src/xenia/memory.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1195 — [GPU] Publish the ring read pointer write-back every 8 packets, not once per burst

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1195](https://github.com/xenia-canary/xenia-canary/pull/1195); created 2026-08-30T04:48:12Z; updated 2026-08-30T12:49:05Z; author `peerloomllc`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `ab349b475d4d82dd2a4330b2e2eb46332006afce`.
- Scope / reason / applicability: Ring read-pointer publication cadence differs from Edge RB_BLKSZ approach; test guest-visible progress.
- Classification: candidate for later port; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-011**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/pm4_command_processor_implement.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-011 decision (2026-09-24): not adopted; the Edge RB_BLKSZ version (29fcaeac3) was ported instead, publishing at the guest-programmed block size rather than every 8 packets.

### xenia-canary/xenia-canary #1227 — [Kernel] Reconcile the guest dispatch header on native object lookup

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1227](https://github.com/xenia-canary/xenia-canary/pull/1227); created 2026-09-10T20:22:20Z; updated 2026-09-13T20:21:27Z; author `Gliniak`.
- Upstream status: **merged upstream**. Merge commit `dcf2994ea1d604e841b5553b2bf941b809e36e79`.
- Scope / reason / applicability: Sync guest dispatcher headers and host event/semaphore state; handle-reuse and wait races require adaptation.
- Classification: candidate for later port; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-014**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xevent.cc`; `src/xenia/kernel/xevent.h`; `src/xenia/kernel/xobject.cc`; `src/xenia/kernel/xobject.h`; `src/xenia/kernel/xsemaphore.cc`; `src/xenia/kernel/xsemaphore.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-014 adoption (2026-09-27): **ported, adapted**. No follow-up fixes or regression reports upstream since the merge (commits to the three files and issue search checked). Adaptations: `GetNativeObject` records every object it creates over guest memory (`SetNativePointer`) instead of each `InitializeNative`; the synchronization-event `WaitCallback` records the host state (`Event::IsSignaled`, `NtQueryEvent`) rather than assuming a reset, because the callback can run after a later `Set`; `NtQueryEvent`'s state uses the same query instead of a wait and re-set; `SignalAndWait` records the signal first through `BeginSignal`/`CancelSignal`; an over-limit guest semaphore count is corrected in the header rather than left there. It also fixed a local bug: `XEvent::Initialize` never stored `manual_reset`, so `NtQueryEvent` reported notification events as synchronization events. Tests: `kernel_tests [object_header]`. See [kernel objects](kernel-objects.md).

### xenia-canary/xenia-canary #1216 — [XAM] Fixed writing to packages via XamContentFlush

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1216](https://github.com/xenia-canary/xenia-canary/pull/1216); created 2026-09-06T20:19:59Z; updated 2026-09-08T18:09:50Z; author `Gliniak`.
- Upstream status: **merged upstream**. Merge commit `5d4dc8a88abb2965f2933286571f5bfa0b87391d`.
- Scope / reason / applicability: Local XamContentFlush returns success without flushing; persistence contract missing.
- Classification: merged upstream; B. **Adapted 2026-09-26 (RG-GDK-017 part 1).** Canary rewrites its directory package's header file on flush; ReXGlue's packages are plain host directories written through unbuffered Win32 handles, so `ContentManager::FlushContent` instead calls `FlushFileBuffers` on every open guest file under the root and restores the header if it is missing. Adapted further: an unknown root returns `X_ERROR_FILE_NOT_FOUND` (Canary returns `X_STATUS_INVALID_PARAMETER` without completing the overlapped), and every result, failures included, completes the overlapped. Header writes (create, flush, install) go through a flushed temporary and a rename. `NtFlushBuffersFile` flushes its file instead of returning success. Tests: `kernel_tests [content]`. See `docs/content-persistence.md`.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xam/content_manager.cc`; `src/xenia/kernel/xam/content_manager.h`; `src/xenia/kernel/xam/xam_content.cc`; `src/xenia/kernel/xam/xcontent/xcontent_package.h`; `src/xenia/kernel/xam/xcontent/xcontent_package_container.h`; `src/xenia/kernel/xam/xcontent/xcontent_package_directory.cc`; `src/xenia/kernel/xam/xcontent/xcontent_package_directory.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1135 — [XAM] Deliver initial XMP state snapshot to new notification listeners

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1135](https://github.com/xenia-canary/xenia-canary/pull/1135); created 2026-08-04T18:31:43Z; updated 2026-08-25T22:50:53Z; author `jman9511`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `421498c3b38257f98efb043f438b8e28ebf951c9`.
- Scope / reason / applicability: Closed unmerged initial XMP notification proposal, Black Ops II; do not assume universal boot notification semantics.
- Classification: useful research; game-specific; G/H. **Left as watch (RG-GDK-017 part 4, 2026-09-26):** the acceptance criterion adopts it only with evidence, and there is none locally (no title muting its soundtrack at boot has been run). The notification behavior ReXGlue does have is specified and tested in `docs/content-persistence.md`.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xam/xam_notify.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #981 — [XAM] Add mutex for properties vector

- Source: [https://github.com/xenia-canary/xenia-canary/pull/981](https://github.com/xenia-canary/xenia-canary/pull/981); created 2026-04-25T16:43:47Z; updated 2026-08-25T20:01:45Z; author `AdrianCassar`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `555e9a4a456d2a6d80a8811486208f82095fcfe9`.
- Scope / reason / applicability: Profile properties mutex; local profile lifetime and thread-safety audit required.
- Classification: relevant but pending upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xam/user_profile.h`; `src/xenia/kernel/xam/user_tracker.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #5 — UserProfile thread-safety (or lack thereof)

- Source: [https://github.com/xenia-canary/xenia-canary/issues/5](https://github.com/xenia-canary/xenia-canary/issues/5); created 2020-01-09T18:16:36Z; updated 2020-02-23T17:12:45Z; author `emoose`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Longstanding UserProfile thread safety; test concurrent read/write and shutdown.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1220 — Player 2 profile save data lost after crash, but persists through normal restart (Army of Two: The 40th Day)

- Source: [https://github.com/xenia-canary/xenia-canary/issues/1220](https://github.com/xenia-canary/xenia-canary/issues/1220); created 2026-09-08T18:31:00Z; updated 2026-09-08T18:36:48Z; author `iceman24895-ui`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Army of Two player-2 profile lost after crash; crash persistence fixture, not a claimed local bug.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1036 — Regression affecting Koei Warriors games - voice line hang/crash

- Source: [https://github.com/xenia-canary/xenia-canary/issues/1036](https://github.com/xenia-canary/xenia-canary/issues/1036); created 2026-05-31T07:54:39Z; updated 2026-06-10T03:59:39Z; author `MaNE-17`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Koei voice-line hang/crash; separate decode, pacing and callback order regressions.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #600 — Xenia freezes when no audio device is enabled or usable on the system.

- Source: [https://github.com/xenia-canary/xenia-canary/issues/600](https://github.com/xenia-canary/xenia-canary/issues/600); created 2025-04-25T08:39:21Z; updated 2026-01-28T07:02:43Z; author `hourai-branch`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: No usable audio device must not freeze title execution; audio sink recovery case.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-019**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #438 — DJ Hero 2 - Song stems other than the first one are completely broken.

- Source: [https://github.com/xenia-canary/xenia-canary/issues/438](https://github.com/xenia-canary/xenia-canary/issues/438); created 2024-11-30T03:30:58Z; updated 2024-11-30T03:30:58Z; author `portalsam1`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: DJ Hero 2 multistream XMA stems; representative decoder workload.
- Classification: game-specific; useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #739 — Sound quality is low/compressed

- Source: [https://github.com/xenia-canary/xenia-canary/issues/739](https://github.com/xenia-canary/xenia-canary/issues/739); created 2025-10-11T11:26:32Z; updated 2025-10-21T06:02:35Z; author `RostovShev03`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Audio quality complaint needs PCM comparisons; scalar-only #748 may not address it.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #872 — [BUG] win32_high_resolution_timer when false is not honored and is instead force enabled to an extremely agressive 0.5ms

- Source: [https://github.com/xenia-canary/xenia-canary/issues/872](https://github.com/xenia-canary/xenia-canary/issues/872); created 2026-02-04T16:00:54Z; updated 2026-02-05T04:56:41Z; author `chrcoluk`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Windows timer resolution flag not honored; measure native wait/timer behavior.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-015**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.
- RG-GDK-015 (2026-09-27): the underlying problem, guest timing against Windows' default 15.6 ms timer, was measured here: 1-10 ms delays and timed waits took about 15.5 ms and sub-millisecond ones returned at once. **Solved differently**: Canary raises the whole system's timer resolution to 0.5 ms (and #872 is that its off switch is ignored). ReXGlue uses a per-thread high-resolution waitable timer (`CREATE_WAITABLE_TIMER_HIGH_RESOLUTION`), which needs no global change. The switch, `guest_precise_timers`, is honored and tested in both states.

### xenia-canary/xenia-canary #773 — [Aggregate Issue] - Regressions

- Source: [https://github.com/xenia-canary/xenia-canary/issues/773](https://github.com/xenia-canary/xenia-canary/issues/773); created 2025-11-08T23:26:10Z; updated 2026-02-15T16:31:57Z; author `The-Little-Wolf`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Aggregate regression index is an ongoing watch source, not a duplicate-all backlog.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-026**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #754 — [Aggregate Issue] - Unimplemented Kernel Functions

- Source: [https://github.com/xenia-canary/xenia-canary/issues/754](https://github.com/xenia-canary/xenia-canary/issues/754); created 2025-10-28T23:01:15Z; updated 2026-09-12T18:44:49Z; author `The-Little-Wolf`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Missing kernel exports: intersect with actual ReXGlue imports; never add success stubs just to boot.
- Classification: useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-014**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-014 intersection (2026-09-27): Quantum of Solace imports 189 kernel/XAM functions; 7 resolve to `REX_EXPORT_STUB` (which leaves `r3` unchanged, not a status): `XeCryptMd5Init/Update/Final`, `NetDll_XNetServerToInAddr`, `NetDll_getsockopt`, `XamShowFriendsUI`, `CurlOpenTitleBackingFile`. None logged a `STUB` call in the 79 recorded runs. The MD5 set is implemented (`include/rex/kernel/xboxkrnl/xecrypt_md5.h`) with the state layout the title's code at 0x8255B7xx fixes: it reads the digest from the state words at offsets 4-16 after a Final with no output buffer. Xenia Edge's implementation (`b80289403777a1f7664ce83688fd7e64b68a820f`, 2024-08-24) was not ported: its 64-bit count moves those words, its Final leaves the digest only in the buffer, and it leaks an FFmpeg context per call. The network and UI stubs stay: networking is out of scope and neither is reached.

### xenia-canary/xenia-canary #1230 — [HID/SDL] controller_subtypes, and a guitar's whammy read as held down

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1230](https://github.com/xenia-canary/xenia-canary/pull/1230); created 2026-09-11T04:42:03Z; updated 2026-09-12T21:42:53Z; author `peerloomllc`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `ef97e8a70f4f0d0fffa2736789670f6f8164d055`.
- Scope / reason / applicability: Guitar subtype/whammy reporting influences GameInput-to-guest mapping even though upstream patch is SDL.
- Classification: relevant but pending upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-020**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/hid/input.h`; `src/xenia/hid/sdl/sdl_input_driver.cc`; `src/xenia/hid/sdl/sdl_input_driver.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.
- RG-GDK-020 review (2026-09-25): still open, head `ef97e8a70f4f` unchanged; not adopted. The GameInput driver reports every gamepad as subtype 1 because GameInput exposes no XInput subtype; no whammy routing is added. Guitar validation is blocked on hardware. See `docs/gameinput.md`.
- 2026-09-28: the GameInput driver now reports wheel, arcade-stick and flight-stick subtypes from the device's kinds (with upstream ReXGlue `78985dd`, below). Guitars and drums still read as gamepads through GameInput, and #1230 remains unadopted.

### xenia-canary/xenia-canary #1109 — [XAM] Resolve directory-backed package payloads

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1109](https://github.com/xenia-canary/xenia-canary/pull/1109); created 2026-07-22T00:10:53Z; updated 2026-08-25T20:01:44Z; author `jaypfe`.
- Upstream status: **pending upstream**. PR head (not adopted) commit `95f9f68817c9828ba3a28c144916d45d34d44bd1`.
- Scope / reason / applicability: Directory-backed package payload lookup; local extracted content and STFS coexist.
- Classification: relevant but pending upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-017**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/kernel/xam/content_manager.cc`; `src/xenia/kernel/xam/content_manager.h`; `src/xenia/kernel/xam/xam_content.cc`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1038 — [APU] Pace audio subsystem

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1038](https://github.com/xenia-canary/xenia-canary/pull/1038); created 2026-06-02T09:59:22Z; updated 2026-07-08T17:24:33Z; author `oreyg`.
- Upstream status: **merged upstream**. Merge commit `6e5b8324f4101464de0f8c2334edb03cac8826c4`.
- Scope / reason / applicability: Audio pacing helps IdolMaster 2/Madden 06, but review reports missing audio in Shaun White Skateboarding and Way of the Samurai 3 freezes/crashes with multiple clients.
- Classification: regression-related; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/apu/audio_system.cc`; `src/xenia/apu/audio_system.h`; `src/xenia/base/threading.h`; `src/xenia/base/threading_posix.cc`; `src/xenia/base/threading_win.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1029 — [APU] XmaDecoder: do not block in WriteRegister

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1029](https://github.com/xenia-canary/xenia-canary/pull/1029); created 2026-05-26T08:53:00Z; updated 2026-06-01T18:41:44Z; author `oreyg`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `a6e1a418f33efb87126ba1ffde2b0b536818170a`.
- Scope / reason / applicability: Closed unmerged: author withdrew nonblocking WriteRegister change pending a better case. Do not mistake closure for adoption.
- Classification: useful research; rejected; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/apu/xma_decoder.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1134 — d119505_canary_experimental update after error message and crash

- Source: [https://github.com/xenia-canary/xenia-canary/issues/1134](https://github.com/xenia-canary/xenia-canary/issues/1134); created 2026-08-04T17:14:17Z; updated 2026-08-05T01:50:05Z; author `stimpack7`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Windows D3D12 device loss reported on GTX 1660 SUPER driver 596.36: e45c25c good, d119505 bad; FIFA 13–19/NFS Hot Pursuit.
- Classification: regression-related; vendor; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-006**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### xenia-canary/xenia-canary #1131 — [GPU] Edge backports, part I

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1131](https://github.com/xenia-canary/xenia-canary/pull/1131); created 2026-08-03T18:14:23Z; updated 2026-08-04T08:34:43Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `0f2980de442341788c07b282d6fbd6dc689166de`.
- Scope / reason / applicability: Edge backport bundle combines Vulkan parity and shared depth behavior; split host-inappropriate pieces from D3D12 semantic candidates.
- Classification: useful research; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-012**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/d3d12_texture_cache.cc`; `src/xenia/gpu/d3d12/d3d12_texture_cache.h`; `src/xenia/gpu/d3d12/pipeline_cache.cc`; `src/xenia/gpu/d3d12/pipeline_cache.h`; `src/xenia/gpu/draw_util.cc`; `src/xenia/gpu/draw_util.h`; `src/xenia/gpu/dxbc.h`; `src/xenia/gpu/dxbc_shader_translator.cc`; `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_fetch.cc`; `src/xenia/gpu/dxbc_shader_translator_om.cc`; `src/xenia/gpu/spirv_compatibility.h`; `src/xenia/gpu/spirv_shader_translator.cc`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_fetch.cc`; `src/xenia/gpu/spirv_shader_translator_rb.cc`; `src/xenia/gpu/texture_cache.cc`; `src/xenia/gpu/texture_cache.h`; `src/xenia/gpu/texture_util.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.h`; `src/xenia/gpu/vulkan/vulkan_render_target_cache.cc`; `src/xenia/gpu/vulkan/vulkan_render_target_cache.h`; `src/xenia/gpu/vulkan/vulkan_texture_cache.cc`; `src/xenia/gpu/vulkan/vulkan_texture_cache.h`; `src/xenia/gpu/xenos.cc`; `src/xenia/ui/vulkan/vulkan_device.cc`; `src/xenia/ui/vulkan/vulkan_device.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### xenia-canary/xenia-canary #1147 — [GPU/Memory] Edge backports, part 2

- Source: [https://github.com/xenia-canary/xenia-canary/pull/1147](https://github.com/xenia-canary/xenia-canary/pull/1147); created 2026-08-10T06:55:59Z; updated 2026-08-18T06:32:24Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `7cd47947b07de30b649fb4224418a659890eab73`.
- Scope / reason / applicability: Edge backports include memory invalidation/read watches and DXBC counterparts; prerequisite research for memexport, not wholesale Vulkan adoption.
- Classification: useful research; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-011**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_texture_cache.cc`; `src/xenia/gpu/d3d12/d3d12_texture_cache.h`; `src/xenia/gpu/dxbc_shader_translator.h`; `src/xenia/gpu/dxbc_shader_translator_fetch.cc`; `src/xenia/gpu/dxbc_shader_translator_memexport.cc`; `src/xenia/gpu/dxbc_shader_translator_om.cc`; `src/xenia/gpu/primitive_processor.cc`; `src/xenia/gpu/primitive_processor.h`; `src/xenia/gpu/render_target_cache.cc`; `src/xenia/gpu/shared_memory.cc`; `src/xenia/gpu/spirv_shader_translator.cc`; `src/xenia/gpu/spirv_shader_translator.h`; `src/xenia/gpu/spirv_shader_translator_alu.cc`; `src/xenia/gpu/spirv_shader_translator_fetch.cc`; `src/xenia/gpu/spirv_shader_translator_memexport.cc`; `src/xenia/gpu/spirv_shader_translator_rb.cc`; `src/xenia/gpu/texture_cache.cc`; `src/xenia/gpu/texture_cache.h`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.h`; `src/xenia/gpu/vulkan/vulkan_render_target_cache.cc`; `src/xenia/gpu/vulkan/vulkan_texture_cache.cc`; `src/xenia/gpu/vulkan/vulkan_texture_cache.h`; `src/xenia/gpu/xenos.h`; `src/xenia/memory.cc`; `src/xenia/memory.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #276 — 4E4D083D - Soulcalibur V Texture Bug

- Source: [https://github.com/has207/xenia-edge/issues/276](https://github.com/has207/xenia-edge/issues/276); created 2026-09-19T04:18:28Z; updated 2026-09-20T13:38:18Z; author `Satumariko`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Soulcalibur V color/pattern corruption; comments separate async shader artifacts from blue speckles across RTV/ROV/FBO versus FSI. No local diagnosis proved.
- Classification: game-specific; useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-011**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #280 — 4B4E081A Otomedius Excellent background glitch

- Source: [https://github.com/has207/xenia-edge/issues/280](https://github.com/has207/xenia-edge/issues/280); created 2026-09-22T03:52:28Z; updated 2026-09-22T07:49:27Z; author `Satumariko`.
- Upstream status: **open report**. Commit not identified for this report.
- Scope / reason / applicability: Otomedius Excellent ocean background oscillation also reported in Canary; camera-matrix cause is a reporter hypothesis, not established.
- Classification: game-specific; useful research; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-026**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #127 — [GPU] Rewrite hardware occlusion (ZPD) implementation

- Source: [https://github.com/has207/xenia-edge/pull/127](https://github.com/has207/xenia-edge/pull/127); created 2026-03-20T04:12:53Z; updated 2026-04-05T14:15:44Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `397fc9284178b7626e77642c6ea24bfa69a620a8`.
- Scope / reason / applicability: Historical ZPD lifecycle rewrite, followed by more corrections; prefer current running-counter design review rather than this snapshot alone.
- Classification: useful research; relevant and merged upstream; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-010**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/command_processor.cc`; `src/xenia/gpu/command_processor.h`; `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.cc`; `src/xenia/gpu/d3d12/d3d12_zpd_query_pool.h`; `src/xenia/gpu/d3d12/pipeline_cache.cc`; `src/xenia/gpu/d3d12/pipeline_cache.h`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/pm4_command_processor_implement.h`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.cc`; `src/xenia/gpu/vulkan/vulkan_pipeline_cache.h`; `src/xenia/gpu/vulkan/vulkan_zpd_query_pool.cc`; `src/xenia/gpu/vulkan/vulkan_zpd_query_pool.h`; `src/xenia/gpu/xenos_report_controller.cc`; `src/xenia/gpu/xenos_report_controller.h`; `src/xenia/gpu/xenos_zpd_report.h`; `src/xenia/ui/config_helpers.h`; `src/xenia/ui/imgui_performance_dialog.cc`; `src/xenia/ui/imgui_performance_dialog.h`; `src/xenia/ui/vulkan/functions/device_1_2_ext_host_query_reset.inc`; `src/xenia/ui/vulkan/vulkan_device.cc`; `src/xenia/ui/vulkan/vulkan_device.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #143 — [GPU] Add a query sample count saturation cvar for flare tuning

- Source: [https://github.com/has207/xenia-edge/pull/143](https://github.com/has207/xenia-edge/pull/143); created 2026-04-20T00:52:28Z; updated 2026-04-21T05:06:55Z; author `goldislead`.
- Upstream status: **merged upstream**. Merge commit `c9a6327ad136f82920df547afc9e552424b863ac`.
- Scope / reason / applicability: Merged empirical sample-count saturation tuning was later removed by running-counter work; do not restore a universal 0.9 multiplier.
- Classification: useful research; obsolete workaround; G/H applicability. **Not adopted**: Canary #1218 removed `occlusion_query_saturation` and RG-GDK-010 ported #1218 without it.
- Source files: `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/xenos_zpd_report.h`.
- Regression evidence: Unknown/not established for ReXGlue. Run the issue-specific regression suite and relevant vendor cases.

### has207/xenia-edge #145 — [GPU] Tune strict ZPD retirement and add optional fast ZPD path

- Source: [https://github.com/has207/xenia-edge/pull/145](https://github.com/has207/xenia-edge/pull/145); created 2026-04-23T14:02:40Z; updated 2026-04-23T15:21:34Z; author `goldislead`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `5d31dea2343fcd60973e17bb89fe0851cca2e1fc`.
- Scope / reason / applicability: Closed unmerged strict/fast tuning; proposal itself notes opt-in report trust regresses some games.
- Classification: experimental; rejected; G/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-010**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/command_processor.cc`; `src/xenia/gpu/command_processor.h`; `src/xenia/gpu/d3d12/d3d12_command_processor.cc`; `src/xenia/gpu/d3d12/d3d12_command_processor.h`; `src/xenia/gpu/gpu_flags.cc`; `src/xenia/gpu/gpu_flags.h`; `src/xenia/gpu/vulkan/vulkan_command_processor.cc`; `src/xenia/gpu/vulkan/vulkan_command_processor.h`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #194 — [GPU] ROV ZPD - Reduces counter atomic pressure

- Source: [https://github.com/has207/xenia-edge/pull/194](https://github.com/has207/xenia-edge/pull/194); created 2026-06-25T02:25:10Z; updated 2026-06-25T06:01:55Z; author `goldislead`.
- Upstream status: **closed unmerged**. PR head (not adopted) commit `9376b083bf8937a1bbc6fbff31cecc4c773a2c30`.
- Scope / reason / applicability: Closed unmerged ROV atomic-pressure optimization; no accepted SPIR-V counterpart established here.
- Classification: useful research; rejected; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-010**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: `src/xenia/gpu/hlsl_shader_translator.cc`.
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #164 — [Regression] Dark Souls broken audio & hangs in menu after release 8aa50e0

- Source: [https://github.com/has207/xenia-edge/issues/164](https://github.com/has207/xenia-edge/issues/164); created 2026-05-14T08:29:01Z; updated 2026-05-16T13:06:33Z; author `shinra-electric`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Dark Souls audio/menu hangs: 3341c7a good, 8aa50e0 bad; reporter later confirms recovery. The follow-up is Edge `b0a1ea5f8` ("Default-init ClientSlot instead of memset over std::mutex", 2026-05-16): 8aa50e0 put a `std::mutex` in the struct the constructor `memset`s. The RG-GDK-018 port keeps its mutexes out of that struct.
- Classification: regression-related; B/H applicability. Cause reviewed and avoided in the RG-GDK-018 adaptation.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #107 — Audio regression in Shin Sangoku Musou 4 Special

- Source: [https://github.com/has207/xenia-edge/issues/107](https://github.com/has207/xenia-edge/issues/107); created 2026-02-17T13:12:06Z; updated 2026-04-16T03:49:05Z; author `QtFun`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Shin Sangoku Musou 4 Special audio closed as fixed; comments distinguish decoder recovery from separate Koei dialogue crashes.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #111 — Midnight Club: Los Angeles audio regression

- Source: [https://github.com/has207/xenia-edge/issues/111](https://github.com/has207/xenia-edge/issues/111); created 2026-02-21T03:14:45Z; updated 2026-02-21T14:41:32Z; author `MS-64`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: Midnight Club LA audio recovery reported; SCDA still had dedicated-thread/BGM issues. Different title outcomes must remain separate.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #113 — Splinter Cell: Double Agent - Big sound regressions

- Source: [https://github.com/has207/xenia-edge/issues/113](https://github.com/has207/xenia-edge/issues/113); created 2026-02-21T19:36:27Z; updated 2026-02-22T07:51:39Z; author `TGP482`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: SCDA music/dialogue stops and scene softlock after 5376439; keep dedicated-XMA-thread settings in reproduction.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

### has207/xenia-edge #120 — 007 Legends - Audio regression

- Source: [https://github.com/has207/xenia-edge/issues/120](https://github.com/has207/xenia-edge/issues/120); created 2026-02-27T19:29:38Z; updated 2026-03-02T10:25:03Z; author `TGP482`.
- Upstream status: **closed report**. Commit not identified for this report.
- Scope / reason / applicability: 007 Legends buzzing/muting after c3d1d3d; reporter confirmed later recovery. Preserve menu and resume-game regression scenes.
- Classification: regression-related; B/H applicability. No adoption by this documentation change.
- Adaptation and validation owner: **RG-GDK-018**, whose complete issue body specifies files, tests and acceptance gates.
- Source files: Report; inspect linked commit/reproducer before implementation..
- Regression evidence: Known hazard or regression is described above and in the linked discussion; reproduce independently.

## Commit-only Edge inventory

No associated PR/issue is asserted unless linked in the notes. Commit messages and diffs are authoritative for the change, not independent proof of compatibility.

### NtOpenFile argument ABI

- Source: [has207/xenia-edge `887beea6979fc1ee8580deb755d771c61e0039ad`](https://github.com/has207/xenia-edge/commit/887beea6979fc1ee8580deb755d771c61e0039ad); 2026-09-20T00:29:06+09:00; Herman S..
- Game/scope: Dead Rising. Classification: General correctness; B.
- Reason / required adaptation / tests: Restore ShareAccess argument and forward it; test PPC r7 versus r8.
- ReXGlue issue: **RG-GDK-003**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/kernel/xboxkrnl/xboxkrnl_io.cc`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Storage request timing

- Source: [has207/xenia-edge `5e9eae601b16757e49eeb83ab0f4d2945809f401`](https://github.com/has207/xenia-edge/commit/5e9eae601b16757e49eeb83ab0f4d2945809f401); 2026-09-17T01:33:24+09:00; Herman S..
- Game/scope: UEFA Champions League 2006–2007 / 45410811. Classification: General compatibility hypothesis; C/G/H.
- Reason / required adaptation / tests: Replaces rejected #275 allocation cap; scheduler-dependent. Test sync/async completion and memory pressure; no global delay transplant.
- ReXGlue issue: **RG-GDK-016**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/emulator.cc`, `src/xenia/kernel/guest_scheduler.cc`, `src/xenia/kernel/guest_scheduler.h`, `src/xenia/kernel/xfile.cc`, `src/xenia/kernel/xfile.h`, `src/xenia/vfs/device.cc`, `src/xenia/vfs/device.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Charge seek only for non-contiguous read

- Source: [has207/xenia-edge `e987fd7f502bc8f0f42345b1855169a2fe7afc25`](https://github.com/has207/xenia-edge/commit/e987fd7f502bc8f0f42345b1855169a2fe7afc25); 2026-09-17T20:52:56+09:00; Herman S..
- Game/scope: Storage workloads. Classification: Regression refinement; B/H.
- Reason / required adaptation / tests: Review together with 5e9eae601; sequential reads must not pay a false seek cost.
- ReXGlue issue: **RG-GDK-016**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/kernel/xfile.cc`, `src/xenia/vfs/device.cc`, `src/xenia/vfs/device.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### XMA loop frame boundary

- Source: [has207/xenia-edge `5dd1cdbbf548745a15714c158b15aad128530440`](https://github.com/has207/xenia-edge/commit/5dd1cdbbf548745a15714c158b15aad128530440); 2026-09-12T15:47:46+09:00; Herman S..
- Game/scope: Tekken Tag 2. Classification: General correctness; B/H.
- Reason / required adaptation / tests: `GetPacketInfo` reports the first frame at or after the requested offset and `Decode` adopts it on a loop restart. Ported as is into `src/audio/xma_context.cpp` (same algorithm; `GetPacketInfo` made static for tests). Tests: `unit_tests "XMA packet walk resolves an offset to the next frame boundary"` and `"XMA loop_start one bit before a frame loops like an exact loop_start"`; both fail with the change disabled. See `docs/xma-audit.md`.
- ReXGlue issue: **RG-GDK-018**; status: ported 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`, `src/xenia/apu/xma_context_new.h`.
- Known regressions: none known upstream. Not run on Tekken Tag 2 locally (no fixture).

### XMA header across packet boundary

- Source: [has207/xenia-edge `adf56b76c434fd97876fd79ffcde65b7ff90c8e6`](https://github.com/has207/xenia-edge/commit/adf56b76c434fd97876fd79ffcde65b7ff90c8e6); 2026-09-06T16:30:48+09:00; Herman S..
- Game/scope: Split frame streams. Classification: General correctness; B/H.
- Reason / required adaptation / tests: A frame whose 15-bit header crosses the packet end is counted with size 0, so it takes the split-header path instead of being skipped; previously only XMA2 packet headers covered this. Ported as is. Tests: `unit_tests "XMA packet walk counts a frame whose header crosses the packet end"` and `"XMA split frame headers decode every frame for XMA1 and XMA2 packets"`; both fail with the change disabled.
- ReXGlue issue: **RG-GDK-018**; status: ported 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`.
- Known regressions: none known upstream. Not run on the musou titles locally (no fixture).

### XMA sub-stream skip chain past frameless packets

- Source: [has207/xenia-edge `9d8210b32`](https://github.com/has207/xenia-edge/commit/9d8210b32); 2026-08-24; Herman S..
- Game/scope: LEGO Star Wars 3, LOTR, Batman 2 opening cutscenes. Classification: General correctness; B.
- Reason / required adaptation / tests: `GetNextPacketReadOffset` follows a frameless packet's own skip count, so interleaved sub-streams do not merge. Ported as is. Test: `unit_tests "XMA next-packet search follows the sub-stream skip chain"`; fails with the change disabled.
- ReXGlue issue: **RG-GDK-018**; status: ported 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`.
- Known regressions: none known upstream. Not run on LEGO titles locally (no fixture).

### XMA work loop drains the current frame

- Source: [has207/xenia-edge `052365bc0`](https://github.com/has207/xenia-edge/commit/052365bc0); 2026-08-25; Herman S..
- Game/scope: NBA Live 06 menu sfx deadlock. Classification: General correctness; B.
- Reason / required adaptation / tests: `Work()` keeps consuming until the current frame is out even after the input runs out, since nothing else would deliver the remainder. Ported as is. Test: `unit_tests "XMA work drains the current frame after the input runs out"`; seven XMA tests fail with the change disabled.
- ReXGlue issue: **RG-GDK-018**; status: ported 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`.
- Known regressions: none known upstream.

### XMA work loop no-progress guard

- Source: [has207/xenia-edge `ade7e610b`](https://github.com/has207/xenia-edge/commit/ade7e610b) (duplicate `836307862`); 2026-04-06; Herman S..
- Game/scope: Halo 4, Tomb Raider. Classification: General correctness; B.
- Reason / required adaptation / tests: `Work()` stops when a pass neither moved the input nor produced a frame, instead of spinning with the context lock held. Adapted: a buffer swap and priming the local start-padding carry also count as progress, because ReXGlue's realignment makes a one-frame loop decode at an unchanged offset. Tests: `unit_tests "XMA work returns when a looping frame keeps failing"` (hangs with the change disabled) and `"XMA single-frame loop keeps producing audio"`.
- ReXGlue issue: **RG-GDK-018**; status: ported (adapted) 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`.
- Known regressions: Edge's earlier stall check false-positived on multi-pass consume and broke Tomb Raider looping; `ade7e610b` is that fix, and the port includes it (only passes with nothing pending are checked).

### XMA output buffer invalidation

- Source: xenia-canary `7e98ae6de` (Gliniak, 2026-05-25), `09dbe2cd3` (oreyg, 2026-05-21), `505697f98` (oreyg, 2026-05-25), as merged into has207/xenia-edge.
- Game/scope: 565507E4 boot hardlock; NFS Carbon and Most Wanted stall detection. Classification: Compatibility; B.
- Reason / required adaptation / tests: The local context predates these refinements of the imported AC6 context. Ported as is: invalidate only a full ring, reset write to read when a kick starts with no valid input, and invalidate an empty ring after a consume-only pass. Tests: `unit_tests "XMA output stays valid when a kick releases nothing"`, `"XMA kick starved of input resets write to read"`, `"XMA consume-only kick drains the frame left by a full buffer"`; the first two fail with the old rule.
- ReXGlue issue: **RG-GDK-018**; status: ported 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/xma_context_new.cc`.
- Known regressions: none known upstream (no reverts through Edge `5dd1cdbbf`). Not run on the named titles locally.

### Retire DXBC for SPIR-V→DXIL transfers

- Source: [has207/xenia-edge `c00e7aead23d03599c7a101f8ae96ca4bfc7fe8c`](https://github.com/has207/xenia-edge/commit/c00e7aead23d03599c7a101f8ae96ca4bfc7fe8c); 2026-08-26T02:34:29+09:00; Herman S..
- Game/scope: General D3D12. Classification: Architectural experiment; C/H.
- Reason / required adaptation / tests: Broad source removal cannot be cherry-picked into ReXGlue DXBC; vendor shader corpus required.
- ReXGlue issue: **RG-GDK-007**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/app/CMakeLists.txt`, `src/xenia/app/xenia_main.cc`, `src/xenia/gpu/CMakeLists.txt`, `src/xenia/gpu/d3d12/CMakeLists.txt`, `src/xenia/gpu/d3d12/d3d12_command_processor.cc`, `src/xenia/gpu/d3d12/d3d12_command_processor.h`, `src/xenia/gpu/d3d12/d3d12_render_target_cache.cc`, `src/xenia/gpu/d3d12/d3d12_render_target_cache.h`, `src/xenia/gpu/d3d12/d3d12_texture_cache.cc`, `src/xenia/gpu/d3d12/d3d12_texture_cache.h`, `src/xenia/gpu/d3d12/pipeline_cache.cc`, `src/xenia/gpu/dxbc.h`, `src/xenia/gpu/dxbc_shader.cc`, `src/xenia/gpu/dxbc_shader.h`, `src/xenia/gpu/dxbc_shader_translator.cc`, `src/xenia/gpu/dxbc_shader_translator.h`, `src/xenia/gpu/dxbc_shader_translator_alu.cc`, `src/xenia/gpu/dxbc_shader_translator_fetch.cc`, `src/xenia/gpu/dxbc_shader_translator_memexport.cc`, `src/xenia/gpu/dxbc_shader_translator_om.cc`, `src/xenia/gpu/metal/msl_shader.h`, `src/xenia/gpu/spirv_shader_translator_alu.cc`, `src/xenia/ui/d3d12/CMakeLists.txt`, `src/xenia/ui/d3d12/d3d12_api.h`, `src/xenia/ui/d3d12/d3d12_provider.cc`, `src/xenia/ui/d3d12/d3d12_provider.h`, `src/xenia/ui/imgui_debug_dialog.cc`, `src/xenia/ui/imgui_debug_dialog.h`, `src/xenia/ui/vulkan/CMakeLists.txt`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Remove Edge dxcompiler.dll dependency

- Source: [has207/xenia-edge `6e9cb7e3f6f5119aca7cf5a43c84b2abb1b78bec`](https://github.com/has207/xenia-edge/commit/6e9cb7e3f6f5119aca7cf5a43c84b2abb1b78bec); 2026-08-27T00:02:48+09:00; Herman S..
- Game/scope: General D3D12. Classification: Architecture evidence; C.
- Reason / required adaptation / tests: Do not confuse Microsoft DXC adoption with Edge current shader pipeline.
- ReXGlue issue: **RG-GDK-007**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `assets/locale/ar/xenia.po`, `assets/locale/bn/xenia.po`, `assets/locale/cs/xenia.po`, `assets/locale/da/xenia.po`, `assets/locale/de/xenia.po`, `assets/locale/el/xenia.po`, `assets/locale/es/xenia.po`, `assets/locale/fa/xenia.po`, `assets/locale/fi/xenia.po`, `assets/locale/fr/xenia.po`, `assets/locale/hi/xenia.po`, `assets/locale/hr/xenia.po`, `assets/locale/hu/xenia.po`, `assets/locale/id/xenia.po`, `assets/locale/it/xenia.po`, `assets/locale/ja/xenia.po`, `assets/locale/ko/xenia.po`, `assets/locale/nl/xenia.po`, `assets/locale/pl/xenia.po`, `assets/locale/pt_BR/xenia.po`, `assets/locale/ru/xenia.po`, `assets/locale/sk/xenia.po`, `assets/locale/sr/xenia.po`, `assets/locale/sv/xenia.po`, `assets/locale/ta/xenia.po`, `assets/locale/th/xenia.po`, `assets/locale/tl/xenia.po`, `assets/locale/tr/xenia.po`, `assets/locale/uk/xenia.po`, `assets/locale/ur/xenia.po`, `assets/locale/vi/xenia.po`, `assets/locale/xenia.pot`, `assets/locale/zh_CN/xenia.po`, `assets/locale/zh_TW/xenia.po`, `src/xenia/gpu/d3d12/dxc_compiler.cc`, `src/xenia/gpu/d3d12/dxc_compiler.h`, `src/xenia/gpu/d3d12/pipeline_cache.cc`, `src/xenia/gpu/d3d12/pipeline_cache.h`, `src/xenia/gpu/spirv_to_dxil_compiler.cc`, `src/xenia/gpu/spirv_to_dxil_compiler.h`, `src/xenia/ui/d3d12/d3d12_api.h`, `src/xenia/ui/d3d12/d3d12_provider.cc`, `src/xenia/ui/d3d12/d3d12_provider.h`, `src/xenia/ui/redist_installer_wx.cc`, `src/xenia/ui/redist_installer_wx.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Publish shader validity before translated flag

- Source: [has207/xenia-edge `462a1ac855d295a87f0c0fab232205eef9359d5c`](https://github.com/has207/xenia-edge/commit/462a1ac855d295a87f0c0fab232205eef9359d5c); 2026-09-18T09:26:08+09:00; Herman S..
- Game/scope: Async compilation. Classification: General correctness; B/H.
- Reason / required adaptation / tests: the local translator set `is_translated_` before `is_valid_` and before the D3D12 wrapper finished (binding layout UIDs, disassembly), while `ConfigurePipeline` and `PrepareRuntimeDescriptionForQueuedCreation` read `is_translated()` without the translation lock (double-checked locking) from the processor and creation threads. Adapted: both flags are atomics (acquire loads); the translator no longer publishes, and `PipelineCache::TranslateAnalyzedShader` calls the new `Translation::PublishTranslated()` (release) last on every exit, so a reader never sees a half-prepared translation. Edge's `TryClaimTranslation` background translation does not exist locally; translations stay serialized by `translation_request_lock_`. No deterministic test; covered by the `gpu_tests [async-pipeline]` stress fixture (RG-GDK-011 part 2), which is not a deterministic reproduction.
- ReXGlue issue: **RG-GDK-011**; status: **ported (adapted)** in RG-GDK-011 part 1.
- Source files: `src/xenia/gpu/metal/metal_command_processor.cc`, `src/xenia/gpu/shader.h`, `src/xenia/gpu/shader_translator.cc`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Pin placeholder pipeline during draws

- Source: [has207/xenia-edge `fe84ec77e87739dd6d26589e35f4c5573e60d271`](https://github.com/has207/xenia-edge/commit/fe84ec77e87739dd6d26589e35f4c5573e60d271); 2026-09-11T00:08:24+09:00; Herman S..
- Game/scope: Async D3D12. Classification: Regression fix; B/H.
- Reason / required adaptation / tests: not applicable. There are no placeholder or interpreter PSOs locally: `IssueDraw` skips a draw while `GetD3D12PipelineByHandle` is null, and a pipeline's `state` only ever goes from null to the real PSO (release store after translation, acquire load at draw and at deferred-list replay), so a handle can never resolve to a different pipeline than the bindings were built for. Revisit if placeholder rendering is adopted.
- ReXGlue issue: **RG-GDK-011**; status: investigated, not applicable (no placeholders).
- Source files: `src/xenia/gpu/d3d12/d3d12_command_processor.cc`, `src/xenia/gpu/d3d12/pipeline_cache.cc`, `src/xenia/gpu/d3d12/pipeline_cache.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Ring pointer RB_BLKSZ publication

- Source: [has207/xenia-edge `29fcaeac3244bed8475b5b8b549a861d743c9c32`](https://github.com/has207/xenia-edge/commit/29fcaeac3244bed8475b5b8b549a861d743c9c32); 2026-08-30T15:50:11+09:00; Herman S..
- Game/scope: General PM4. Classification: General correctness candidate; B/H.
- Reason / required adaptation / tests: local code matched Edge's pre-fix state (write-back once per burst, `read_ptr_update_freq_` in the wrong unit). Ported into `CommandProcessor::ExecutePrimaryBuffer` (local `RingBuffer` reader) and `EnableReadPointerWriteBack`: republish every RB_BLKSZ quadwords, release fence before the store, write-back target re-read each time. Preferred over Canary #1195's fixed 8-packet cadence (closed unmerged) because it follows the guest-programmed block size. Tests: `gpu_tests [ring]` — the write-back advances to within one stride of a WAIT_REG_MEM blocked on the guest (fails without the port), and five ring lengths of bursts wrap while the fixture waits on the write-back for room.
- ReXGlue issue: **RG-GDK-011**; status: **ported** in RG-GDK-011 part 1.
- Source files: `src/xenia/gpu/command_processor.cc`, `src/xenia/gpu/pm4_command_processor_implement.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Guest scheduler starvation hack

- Source: [has207/xenia-edge `83abd43576d99ff5ef651e54272f8e95c077af76`](https://github.com/has207/xenia-edge/commit/83abd43576d99ff5ef651e54272f8e95c077af76); 2026-08-31T10:13:18+09:00; Herman S..
- Game/scope: Guest scheduler. Classification: Experimental; G/H/C.
- Reason / required adaptation / tests: Do not port to static runtime. Requires guest scheduler and measured starvation reproducer.
- ReXGlue issue: **RG-GDK-015**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/kernel/guest_scheduler.cc`, `src/xenia/kernel/guest_scheduler.h`, `src/xenia/kernel/xthread.h`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Case-insensitive content match

- Source: [has207/xenia-edge `16ba84808609b449778b1f00745120851697989c`](https://github.com/has207/xenia-edge/commit/16ba84808609b449778b1f00745120851697989c); 2026-08-31T12:17:58+09:00; Herman S..
- Game/scope: Content packages. Classification: General correctness candidate; B.
- Reason / required adaptation / tests: FATX lookup semantics with mixed case and collisions; preserve Windows paths and UTF-8.
- ReXGlue issue: **RG-GDK-017**; status: investigated, not ported. PR: not identified; issue references appear in the linked roadmap body.
- Source files: `src/xenia/kernel/xam/content_manager.cc`.
- Known regressions: not established locally; preserve upstream follow-ups and run the mapped regression gate.

### Audio client lifetime/lock ordering

- Source: [has207/xenia-edge `8aa50e0e07ed659b26ea06e72c4bc21a07ad9bbb`](https://github.com/has207/xenia-edge/commit/8aa50e0e07ed659b26ea06e72c4bc21a07ad9bbb); 2026-05-13T16:14:22+09:00; Herman S..
- Game/scope: Title teardown. Classification: Regression-prone correctness; B/H.
- Reason / required adaptation / tests: Reviewed with Canary #1214 (lock-order inversion) and Edge #164, whose cause was the new `std::mutex` sitting in a `memset` struct, fixed by Edge `b0a1ea5f8`. Adapted into `src/audio/audio_system.cpp`: the per-client callback mutexes live outside the `memset` `clients_` array; `UnregisterClient` clears the slot under the global lock, waits for an in-flight callback without it, then destroys the driver and frees the callback argument (Edge leaks the argument; after the wait nothing can hold it). The slot stays reserved until teardown ends so `RegisterClient` cannot reuse it early. Unregistering from inside the client's own callback skips the wait. Late `SubmitFrame` and double or out-of-range unregisters are dropped instead of dereferencing a null driver. Tests: `unit_tests [audio][lifetime]`; the wait test fails with the wait removed.
- ReXGlue issue: **RG-GDK-018**; status: ported (adapted) 2026-09-25. PR: not identified.
- Source files: `src/xenia/apu/audio_system.cc`, `src/xenia/apu/audio_system.h`.
- Known regressions: Edge #164 (the `memset` over `std::mutex`, avoided here). A guest that unregisters while holding a guest lock its own callback needs would now wait on that callback; not seen upstream, recorded in `docs/xma-audit.md`.

### XAudio2 audio driver

- Source: has207/xenia-edge `src/xenia/apu/xaudio2` at `5dd1cdbbf` (last driver changes `71dcd5004` volume controls, 2026-05-21; `9371e73d9` "Fix audio crashes during shutdown", 2026-02-09); Herman S. and upstream Xenia authors.
- Game/scope: all titles (host audio output); Canary #600 (freeze with no usable audio device, open). Classification: platform replacement plus correctness; A/B.
- Reason / required adaptation / tests: Structure kept (COM MTA owner thread, one source-voice buffer per guest frame, `OnBufferEnd` releases the client semaphore, stop the engine before destroying voices). Adapted: Windows SDK `xaudio2.h` and inbox XAudio2 2.9 instead of hand-declared 2.7/2.8 interfaces; default device, channels and rate so the virtual audio client follows device changes; the SDL output's conversion, fold, mix, gain and mute; device loss through `OnCriticalError`, failed submits and a stall watchdog, with the dead voice's frames and any frame submitted without a device released on a frame-rate clock and the engine recreated. Not ported: volume cvar and UI (local mix API instead), `SetFrequencyRatio` from the guest time scalar (the SDL output has no equivalent). Selected with `audio_backend = "xaudio2"`; SDL stays default. Tests: `unit_tests [audio][xaudio2]`, `[audio][conversion]`. See `docs/audio-output.md`.
- ReXGlue issue: **RG-GDK-019**; status: ported (opt-in) 2026-09-26. PR: not identified.
- Source files: `src/xenia/apu/xaudio2/xaudio2_audio_driver.cc`, `xaudio2_audio_driver.h`, `xaudio2_audio_system.cc`, `xaudio2_api.h`.
- Known regressions: Canary #600 (Xenia's drivers freeze with no device) is what the clock pacing addresses; not reproduced on hardware here. Canary #739 (low quality) concerns sample conversion, tracked with PR #748 in `docs/xma-audit.md`.

### Native Win32 window (last version before Qt)

- Source: [has207/xenia-edge `213dcc2675806bf1e61291dd6ed0bb897c8d2fff`](https://github.com/has207/xenia-edge/commit/213dcc2675806bf1e61291dd6ed0bb897c8d2fff) ("[UI] Convert to Qt"; the Win32 files were deleted by the next commit, `92f5b712d`); 2025-10-04; Herman S..
- Game/scope: all titles (host windowing). Classification: platform replacement; A/B.
- Reason / required adaptation / tests: The Win32 window and app context this SDK's `Window` API came from. Ported as `src/ui/window_win.cpp` and `windowed_app_context_win.cpp`, selected with `ui_backend = "win32"` (default stays SDL). Adapted: no native menus (the SDL window has none), the listeners' close veto, minimize and restore events, raw-input relative mouse with cursor clipping, warp to center, `WM_CHAR` and IME gated on text input, the `monitor` cvar with the primary display first, horizontal wheel, and process DPI awareness set in code. Tests: `unit_tests [ui][win32]`, `gpu_tests [gpu][win32]`. See `docs/windowing.md`.
- ReXGlue issue: **RG-GDK-021**; status: ported (opt-in) 2026-09-25. PR: not identified.
- Source files: `src/xenia/ui/window_win.cc`, `src/xenia/ui/window_win.h`, `src/xenia/ui/windowed_app_context_win.cc`, `src/xenia/ui/windowed_app_context_win.h`.
- Known regressions: none known upstream (the files were retired for Qt, not for defects). Not established locally on titles, multiple DPIs, or AMD and Intel.

### Gaming Runtime lifecycle and PC packaging (no upstream source)

- Source: none. Checked 2026-09-26: xenia-canary and has207/xenia-edge have no GDK, Gaming Runtime or `MicrosoftGame.config` work (`gh search prs/issues` for "GDK", "MicrosoftGame.config", "XGameRuntime"; Canary #769 is GTK's `GDK_BACKEND`, unrelated). Built from Microsoft's GDK 2604 documentation (`XGameRuntimeInitializeWithOptions`, `XPackageIsPackagedProcess`, MicrosoftGame.config reference) and the installed edition 260404 (`XGameErr.h`, `GameConfigSchema.xsd`, `makepkg`, `wdapp`).
- Game/scope: all GDK titles (host deployment). Classification: platform addition; A.
- Reason / tests: `rex::system::GamingRuntime`, `ReXApp`'s `gaming_runtime` policy, `rexglue init gameconfig`, `rexglue_add_game_config`. Tests: `unit_tests [gaming_runtime],[gameconfig]`, `gdk.gameconfig_schema`, `gdk.smoke_*`. See `docs/gdk-packaging.md`.
- ReXGlue issue: **RG-GDK-022**; status: implemented 2026-09-26.
- Known regressions: none upstream. Local finding: a `rexruntime.dll` from an SDK installed elsewhere on `PATH` can satisfy a title's import (0xC0000139); deploy the title's own DLLs beside it.

### PIX host markers (Edge debug-marker plumbing, PixEvents encoding)

- Source: [has207/xenia-edge `c777013dc48c1295b2683aa6940bd880188da334`](https://github.com/has207/xenia-edge/commit/c777013dc48c1295b2683aa6940bd880188da334) ("[GPU/Debug] Add debug markers for renderdoc/pix frame capture", 2025-12-06, Herman S.), already present here as plumbing (`gpu_debug_markers`, `PushDebugMarker`, deferred `BeginEvent`) with no call sites. Follow-up [`3d4be9fa076a398cc5a4e61ec5521235d520bbcd`](https://github.com/has207/xenia-edge/commit/3d4be9fa076a398cc5a4e61ec5521235d520bbcd) ("Additional GPU debug annotations": render target transfers, texture loads, shared memory, PM4 packets) reviewed 2026-09-27 and not ported. Event encoding from [microsoft/PixEvents `b0caa735f8510f4ff60c29ef1c88101620defc5e`](https://github.com/microsoft/PixEvents/tree/b0caa735f8510f4ff60c29ef1c88101620defc5e/include) (MIT; `PIXEventsCommon.h`, `PIXEvents.h`, `pix3_win.h`), reimplemented rather than vendored.
- Game/scope: all titles (host diagnostics). Classification: A (diagnostics; no guest-visible change).
- Reason / required adaptation / tests: Edge passes labels with the legacy ANSI metadata, which PIX 2603 shows as "deprecated - use pix3.h", and its Push/Pop pairs break when a submission ends inside a region. Here the labels use the `WINPIX_EVENT_PIX3BLOB_V2` blob, regions closed by a submission end are tracked (`DebugMarkerRegions`), markers cover queue submissions, draws, resolves and swaps, and PIX launching the process turns them on. `d3d12_capture_frame` (no Edge counterpart) takes a programmatic capture of one guest frame. Tests: `unit_tests [debug_markers]`, `gpu.pix_capture` (`scripts/pix_capture_fixture.ps1`). See [baseline capture](baseline-capture.md#pix-for-windows-captures-rg-gdk-028).
- ReXGlue issue: **RG-GDK-028**; status: implemented 2026-09-27.
- Known regressions: none found upstream (Edge issues and PRs searched for "debug markers"). Edge's finer annotations remain a candidate; they need the same balancing before use.

### has207/xenia-edge `de8e60601` — [GPU] Wait for the real pipeline where an async stand-in would persist

- Source: [has207/xenia-edge `de8e60601c1f8182757e92b455d2299799573202`](https://github.com/has207/xenia-edge/commit/de8e60601c1f8182757e92b455d2299799573202), 2026-09-26, Herman S. Upstream reports it fixing NFS The Run's road bushes, cold-cache rendering in Soul Calibur V, road flashing in FM3 and vertical rain in Halo Reach.
- Game/scope: all titles with `async_shader_compilation` (on by default). Classification: A (correctness).
- Local evidence: a draw whose pipeline was still being created was skipped. That self-heals only for passes redrawn every frame; a one-off render to a texture lost its output for good (RG-GDK-028's PIX fixture read back 0 until async compilation was turned off).
- Adaptation: ReXGlue has no placeholder pipeline, so only the wait is ported. The placeholder colour-mask half does not apply. `RenderTargetCache` records the render target each draw goes into and the last frames it was drawn in. For a target not drawn in the last 4 frames, a small one (at most 2 tiles wide) or a memexport draw, `IssueDraw` creates the queued pipelines on the processor thread and waits for the rest (`PipelineCache::AwaitQueuedPipelines`). Other draws are still skipped while compiling.
- Tests: `gpu_tests [async-pipeline]` "A one-off draw under async compilation waits for its pipeline" (small and wide targets). Both read back 0 with the wait removed.
- Known regressions: none reported upstream. Expected cost: a hitch on the first draw into a new render target while its pipeline compiles.

### has207/xenia-edge 2026-09-25/26 commits reviewed and not ported

- [`77f2cca80`](https://github.com/has207/xenia-edge/commit/77f2cca80) "Copy resolve output into guest RAM when the CPU accesses it" and [`935e03876`](https://github.com/has207/xenia-edge/commit/935e03876) "Copy resolve output without holding the global lock": a redesign of Edge's resolve read-watch machinery (about 2,000 lines, D3D12, Vulkan and memory), which ReXGlue does not have (it has the `readback_resolve` modes). **Watch**: a separate issue if adopted, after upstream settles (patched again the day after).
- [`776dda2e3`](https://github.com/has207/xenia-edge/commit/776dda2e3) "Unmark a whole scaled resolve when the CPU writes into it" (Tekken Tag Tournament 2 at 2x): ReXGlue has the old per-page unmark, but the fix records extents only where Edge's resolve mirroring (from `77f2cca80`) places output. **Watch** with the redesign; affects resolution scaling only.
- [`12e3b4223`](https://github.com/has207/xenia-edge/commit/12e3b4223) "Keep the audio pump off the global lock and make up late pumps": it changes Edge's deadline-paced audio pump, which ReXGlue's semaphore-driven worker does not use. The ReXGlue worker takes the global lock only briefly per callback. **Not ported** without local stutter evidence.
- [`13e380a1d`](https://github.com/has207/xenia-edge/commit/13e380a1d) "Fix loading aliased bool values": Edge's legacy cvar aliases and per-game configs. **Not applicable** (a different cvar system; ADR-009 instead).

### has207/xenia-edge 2026-09-19 to 2026-09-23 commits (fork sync, reviewed 2026-09-29)

Edge `edge` at `12e3b4223dd4c2e41d57ea4b4477546affe4ce10`, compared with
ReXGlue `main` on 2026-09-29. Commits already recorded above (`de8e60601`,
`887beea69`, the Canary PRs #1218/#1238/#1240/#1243) are not repeated.

- [`aac25ad0c`](https://github.com/has207/xenia-edge/commit/aac25ad0c) "Actually apply license_mask to content packages when it is 1" (Herman S., 2026-09-22). Classification A. ReXGlue's `ContentManager::OpenContent` did not apply `license_mask` at all. **Ported:** every opened package gets the mask ORed in (a nonzero mask, as Edge now does). Default 0, so no title changes unless the cvar is set. Test: `kernel_tests` "license_mask grants licenses to opened content". Known regressions: none reported.
- [`74c4e4acb`](https://github.com/has207/xenia-edge/commit/74c4e4acb) "Add missing exports, add missing param name, and more" (The-Little-Wolf, 2026-09-19). ReXGlue already has the export table entries and the `XamParty*` returns. `XamLoaderGetMediaInfo`/`Ex` and `XamLoaderRegisterLaunchRequestCallback` stay stubs: no investigated title calls them. **Watch.**
- Edge `xam_info.cc` `XGetAudioFlags` (present at the pinned commit): returns XConfig's user audio flags, default `0x00010001`. ReXGlue's XConfig already reported that value while `XGetAudioFlags` was a stub. **Adapted:** both read `kXConfigUserAudioFlags`. Test: `kernel_tests` "XGetAudioFlags reports the console's XConfig audio flags". Edge's `avpack == 2` branch is not taken: ReXGlue's `XGetAVPack` is fixed at 6.
- Edge `apu_flags.cc` `volume` (0-100, master volume of the XAudio2 and SDL drivers). **Adapted** as `audio_volume`, applied through `MasterOutputGain()`. Test: `unit_tests [audio][volume]`.
- [`de6556b3c`](https://github.com/has207/xenia-edge/commit/de6556b3c) "Launch a relaunched title under the file's own name": Edge's game library. **Not applicable.**
- [`4fc57ca43`](https://github.com/has207/xenia-edge/commit/4fc57ca43) "Log NtCreateFile paths and results": ReXGlue already logs them. **Already present.**
- [`aebd98b62`](https://github.com/has207/xenia-edge/commit/aebd98b62), [`b7336b560`](https://github.com/has207/xenia-edge/commit/b7336b560), [`9afccb2c8`](https://github.com/has207/xenia-edge/commit/9afccb2c8), [`0c2da08dd`](https://github.com/has207/xenia-edge/commit/0c2da08dd): JIT and emulator debugging aids (user-mode trap logging, module image dumps, `log_lr_*`, launch-data logging). **Not ported**; the fault report (#106) covers ReXGlue's static case.
- [`791440756`](https://github.com/has207/xenia-edge/commit/791440756) "Record stackpoints after the frame is allocated", [`1e640ba49`](https://github.com/has207/xenia-edge/commit/1e640ba49) "Port the backend half of user mode to a64": Edge's x64/a64 JIT backends. **Not applicable** (ADR-004).

Not implemented, with reasons:

- **`XamContentResolve`** (Canary `b8296a9bc`, Edge `2f6baa751`/`3ecd9d36b`): it returns the guest path of a content package, which 007 Legends passes straight to `NtOpenFile` as a file. ReXGlue (like Canary and Edge) stores packages as folders and has no guest path to the content root, so a resolved path could not open either. Legends handles the `X_ERROR_NOT_FOUND` and its autosave works. Revisit if a title needs the path itself.
- **`XGetVideoFlags`**: neither Canary nor Edge implements it, and the meaning of XConfig's `0x00040000` video flags is not established. Left a stub rather than guessed.

### has207/xenia-edge frame limiter: guest vblank pacing (2026-09-29)

- Source: Edge `src/xenia/gpu/graphics_system.cc` frame limiter thread at `12e3b4223dd4c2e41d57ea4b4477546affe4ce10` (Herman S.; history includes `9d9377322`, `ba5fd0f41`, `16fc37fed`). Classification A (correctness of guest vblank timing), all titles.
- Local evidence: with `frame_stats_interval`, Quantum of Solace with its 60 fps patch ran at 33-38 fps with `vsync` on and 65-72 fps with it off. Late frames clustered at about 31 and 47 ms, two and three 15.6 ms Windows timer ticks: the vblank thread slept `Sleep(1 ms)`, which wakes every 15.6 ms, and then fired the missed vblanks in a burst.
- Adaptation: `VblankPacer` fires one vblank per interval and starts a new cadence when more than two intervals late, as Edge does; the thread sleeps on the existing high-resolution waitable timer (`PreciseSleep`) to 0.5 ms before the vblank and yields the rest, instead of Edge's 90 % `NanoSleep`. Edge's `guest_display_refresh_cap` stays ReXGlue's `vsync`; `framerate_limit` and the synthesized `D1MODE_V_COUNTER` are not ported.
- Result: the same menu run went to 56-58 fps, and 45-49 fps where frames take longer than one refresh (the vsync-off run shows p99 18-22 ms there). Tests: `unit_tests [graphics][vblank]`.
- Known regressions: none reported upstream.

### xenia-project/xenia `73c30d87a`: high timer resolution at startup (2026-09-29)

- Source: DrChat, 2018-05-22, "[App] Request high-performance timer resolution on Windows" (`src/xenia/base/main_win.cc`, still in Canary and Edge at `12e3b4223`). Classification A, all titles.
- Local evidence: the ReXGlue entry point never did this, so every millisecond sleep in the SDK woke in 15.6 ms steps. After the vblank pacing fix, Quantum of Solace still spent frames waiting: its render thread spins in D3D's ring-space wait (`sub_820E7098`, found by sampling the thread and mapping host addresses through `PPCFuncMappings`) while the command processor slept in `PM4_WAIT_REG_MEM`, whose `Sleep(wait / 0x100 ms)` lasted 15.6 ms.
- Adaptation: `rex::thread::RequestHighTimerResolution()` sets the finest resolution (`NtQueryTimerResolution` / `NtSetTimerResolution`, 0.5 ms here) in the Windows entry point; ReXApp logs it. Edge's precise-sleep variant of `WAIT_REG_MEM` is not needed at this resolution.
- Result: the same menu run holds 59-60 fps, p95 17.6-18.1 ms (was 45-59 fps, p95 about 31 ms). Test: `unit_tests [core][timer]` (a 1 ms sleep averaged 1.33 ms).
- Known regressions: none; Windows 11 honours the request per process while the window is visible.

### Resolve readback on by default (2026-09-29)

- Sources: xenia-canary `src/xenia/gpu/command_processor.cc` (`UPDATE_from_string(readback_resolve, 2025, 12, 4, 21, "fast")`, `canary_experimental` at review time) and has207/xenia-edge `77f2cca80` (2026-09-25, `readback_resolve` a bool, on by default). Classification A, all titles.
- Local evidence: Blood Stone's first level rendered almost white with the old default `none`; with `--readback_resolve=fast` it renders correctly and still holds 30 fps. Quantum of Solace holds 59-60 fps either way.
- Change: default `readback_resolve` is now `fast`. Edge's on-demand copy redesign (`77f2cca80`, `935e03876`) is still watched separately.

## Upstream ReXGlue since v0.10.0

The fork's base is upstream [rexglue/rexglue-sdk](https://github.com/rexglue/rexglue-sdk) `v0.10.0` (`c94f5eb`). Its `development` branch had 15 commits by 2026-09-27 (head `5cf287f`), reviewed here and ported in groups with the original authors kept. The rest of what upstream released as v0.10.1 is reviewed [below](#upstream-rexglue-v0101-2026-11-06).

| Upstream commit | Area | Status here |
| --- | --- | --- |
| `6319e23` fix(codegen): pack vpkuwus/vpkuhus through vTemp when vD aliases a source (#449) | Codegen | **Ported.** The PPC tests with vD aliasing vA or vB fail on the old builder. The other element-wise vector builders were audited: each reads the element it writes, so aliasing is safe. |
| `0c7b01a` fix(kernel): release leaked event, socket and completion port objects | Kernel | **Ported.** `kernel_tests [leak]` checks that 1,000 create/close cycles leave the process handle count flat (it grew by 1,000 before), and that a bad `NtRemoveIoCompletion` handle returns `X_STATUS_INVALID_HANDLE` (it crashed before). No other unreleased `new X...` sites remain. |
| `01ac2a3` fix(filesystem): truncate overwrites in place and release the CRT file object | Filesystem, content | **Ported.** Merged with RG-GDK-017's durable content changes. Upstream's `unit_tests` VFS overwrite tests fail on the old code. Quantum of Solace writes its save and header on the GDK build. |
| `ee20900` fix(xam): compare content results against X_ERROR_SUCCESS instead of testing them as NTSTATUS | XAM | **Ported, extended** to three more X_RESULT checks: `XamInputGetKeystroke` (X_ERROR_EMPTY counted as success), `XamGetOverlappedResult`, and RG-GDK-017's `FlushContent` header write. |
| `78985dd` feat(input): report the real XInput device subtype | Input | **Ported, GDK first.** SDL and XInput ported as is. The GameInput driver had reported every device as a gamepad; it now takes the subtype from `supportedInput` (wheel, arcade stick, flight stick) and reports no vibration for a device without both body motors. Tests: `unit_tests [gdk][gameinput]`, `[input][gameinput]`. |
| `c695852` feat(input): log joystick hotplug and widen wireless detection | Input (SDL) | **Ported** as is for the SDL fallback. GameInput already reports wireless through `GameInputDeviceWireless`, and its connect log now names kinds, subtype and motors. |
| `3cd7243` feat(input): vibration switch, deadzones, UI blocking, hotplug notices | Shared input layer | **Ported, adapted.** It lives in `InputSystem`, so GameInput, XInput and SDL all get it. Adapted to the fork's input lock: the blocker count and masks are guarded by it, `XN_SYS_INPUTDEVICESCHANGED` is broadcast after it is released, and the connected-user getters are atomic. Fixed: upstream's deadzone compared each axis against a signed projection, so a stick pushed left or down was never cut. Extended: keystrokes are blocked during a dialog too, and those made during it are discarded as it closes. Tests: `unit_tests [deadzone],[vibration],[ui_block],[hotplug]`; the deadzone, masking and keystroke checks fail with upstream's comparison or without the masking. |
| `3f34ffc` feat(input): keyboard passthrough and bound-key keystrokes | Input (keyboard) | **Ported** as is. Key events reach the driver the same way from the Win32 and SDL windows. Limitation, as upstream: a keystroke's character arrives only while text input is active, because both windows deliver `WM_CHAR` only then. Tests: `unit_tests [mnk]` (upstream had none). |
| `289f518` feat(ui): resolve the window size through one cvar order and expose the display size | Window, kernel video mode | **Ported, adapted.** The size order goes into the fork's shared `window_factory.cpp`, so the Win32 and SDL windows agree. `Window::Create` keeps its explicit-size overload for SDK consumers and tests. `GetDisplayPixelSize` is implemented for Win32 from the registry (desktop) mode. The guest video mode follows the same order when `video_mode_*` are unset. |
| `1406e1b` feat(ui): switch the display mode in fullscreen and gate input on window focus | Window, input | **Ported, Win32 implemented.** Win32 switches the mode with `ChangeDisplaySettingsExW(CDS_FULLSCREEN)`, choosing like SDL (`src/ui/display_mode.h`, unit-tested). It restores the desktop mode on leaving fullscreen, on close and on switching away (minimizing, as SDL does). Focus gating is in `ReXApp`, for every input backend. |
| `923c1a5` feat(ui): apply the monitor, window size and fullscreen mode without a restart | Window | **Ported, Win32 implemented.** `ApplyNewMonitor` and `ApplyNewDesiredLogicalSize` for Win32, including while fullscreen. A refresh of an already borderless window keeps the saved placement; upstream's SDL window has no equivalent state to lose. Tests: `unit_tests [ui][win32]`; the size and refresh checks fail without the fullscreen handling. |
| `b971840` feat(logging): log path, directory budget and flush through LogConfig | Logging | **Ported, adapted.** Each run is now one file (no rotation), and `log_max_file_size_mb` and `log_max_files` are gone. Upstream left the new directory budget off, so logs would grow without limit. `ReXApp` sets it to 100 MiB, the old rotation's total, and titles can change it in `OnConfigureLogging`. `PruneLogDirectory` is public for tests. The `const char*` `InitLogging` overload is kept, with nullptr meaning no file: upstream's path-only signature built a path from nullptr, which crashed the 32 GPU tests whose fixture passes one. Tests: `unit_tests [logging]`. Follow-up: a packaged GDK title's install directory is read-only, so `exe_dir/logs` needs a writable default there (RG-GDK-022); `OnConfigureLogging` is the hook. |
| `5cf287f` refactor(rexglue): do not force snake_case project names | CLI | **Ported, fixed.** Upstream's "original" name dropped the separators, so a project named `my_game` by earlier versions would regenerate `rexglue.cmake` pointing at `mygame_manifest.toml` and `mygame_pch.h`. Here the case is kept and words are joined with one underscore, which is also a valid identifier for `REX_DEFINE_APP` (for example `Blood Stone` becomes `Blood_Stone`). Existing names regenerate unchanged. Tests: `unit_tests [rexglue][init]`. |
| `4c13508` deps: update SDL3 to 3.4.14 | Dependency | **Ported.** The pin moves from a development snapshot after 3.4.0 (`8bf3b72`, which had OpenXR GPU and GameSir HIDAPI sources) to the stable `release-3.4.14`. SDL is the fallback for input, audio and the window; all four configurations and Quantum of Solace pass on it. |
| `e9d4425` deps: bump MoltenVK to 1.4.3 | Dependency | **Not applicable** (macOS/Vulkan; ADR-001, ADR-002). |

## Upstream ReXGlue v0.10.1 (2026-11-06)

Upstream released v0.10.1 on `main` as one squash commit (`546a082`) of its `development` work. Its four headline pull requests (#449, #425, #417, #450) are the commits above plus `7f7c92e`. The development commits not reviewed above, as of 2026-11-11:

| Upstream commit | Area | Status here |
| --- | --- | --- |
| `bd833a2` feat(codegen): add sync fences for sync, lwsync, and eieio | Codegen | **Ported.** `sync` and `eieio` emit a `seq_cst` fence, `lwsync` an `acq_rel` fence ([memory barriers](../research/cpu/memory-barriers.md)). Blood Stone regenerated with 6,766 fences holds its 30 fps cap at the same CPU use (3.6 cores in its Istanbul level). PPC tests pass. |
| `ea222e9` fix(codegen): fix register types to be unsigned | Codegen | **Ported** (the `stwcx.`/`stdcx.` fields; no behaviour change on x86-64). |
| `7f7c92e` codegen: migrate stwcx. to std::atomic CAS, drop CR0.so and redundant fences (#450) | Codegen | **Not taken.** This fork's `stwcx.` (RG-GDK-072) also checks the reservation address and sets CR0.SO from XER.SO, and passes the hardware-captured PPC corpus; upstream's drops CR0.SO. Its removal of the fences around `mfmsr`/`mtmsrd` is not shown to be safe here. |
| `5e2dae1` feat(system): add writable_code_segments | Memory | **Not taken.** Recompiled code never runs code from guest memory, so whether code segments are writable only matters to a title that patches its own code. |
| `9dea59f` build: embed a version resource in shipped SDK binaries | Build | **Deferred.** Useful; would also need the `rexgpu-regpu` plugin. |
| `5024759`, `00276db` fix(build): the SDK version and exported `rex::` targets under `add_subdirectory` | Build | **Deferred.** Title projects here use the installed SDK (`find_package`). |
| `7f80718` fix(build): require SSE4.1 on SDK targets independent of the preset | Build | **Covered.** `rexglue_helpers.cmake` sets `-msse4.1` on every title target. |
| `b5e0cf8` fix(build): expose the imgui and xxHash headers public SDK headers include | Build | **Partly covered** (xxHash is public on `rexruntime`); the imgui path is deferred with the `add_subdirectory` fixes. |

Upstream's `development` branch has 326 more commits for 0.11 (head `5940371`, 2026-11-11): a module reorganisation (`rexbase`, `rexkernel`, `rexxbdm`), TLSF in place of o1heap, Vulkan and macOS builds, and many kernel, XAM and memory fixes. It cannot be merged as a whole into this Windows/D3D12 tree; its fixes will be reviewed one by one when 0.11 is released.

## Review process

### Local RG-FIX-002 discovery correction (2026-09-28)

Source: this fork at `d6ccced`; issue
[SDK #32](https://github.com/furqanagwan/rexglue-sdk/issues/32), with title
evidence in [007 #6](https://github.com/furqanagwan/007/issues/6).
This is a local correctness investigation, not an upstream port (external
PR head/merge state: not applicable). Gap segmentation and block discovery
disagree about the end of indirect/direct tail-dispatch segments. The bounded
adaptation revisits unclaimed suffixes and handles explicitly tail-referenced
constant-return shared leaves. It does not enable the rejected broad pointer
scanner, import JIT machinery or add a title-specific global rule.

[Diagnosis and regression record](indirect-function-discovery.md) includes
the pinned XEX identity, object-table storage trace, rejected adjacency scan,
rejected cleanup exemption, return-padding false candidates, positive and
negative synthetic fixtures, and before/after registration and overlap counts.
Other shared tails and switch-only entries remain outside the proven rule;
the title keeps its explicit hints. No upstream regression claim is inferred
from the local result.

Monthly, and before each subsystem port or release: fetch upstream refs into the controlled reference checkout; record date, SHA, merge-base and patch-equivalence comparison. Read new/updated issues and PRs, including closed-unmerged work and regressions. Search subsystem terms plus AMD/NVIDIA/Intel and title IDs. Re-check older open watches. Record explicit classification, affected titles, JIT dependencies, tests and whether a ReXGlue issue is justified. Update existing issues rather than duplicate.

Port only to the implementation fork, retaining author/license and full SHA/PR/issue provenance. Separate general corrections, driver workarounds and title profiles. Run baseline and vendor gates, record results, then merge through normal review. Never auto-merge from Xenia/Canary/Edge or make the runtime depend on the Edge checkout. Keep rejected proposals in this ledger to prevent rediscovery as apparently new fixes.

A watch issue is unblocked by a reproducible local failing fixture plus settled semantics, or by an upstream resolution that survives the same review. Merge upstream alone is not sufficient. The roadmap has one continuous-review issue; narrower pending work is attached to its owning subsystem issue instead of duplicating every upstream request.

## Reproduce source acquisition

From the controlled Edge checkout, fetch read-only references (these create only
local research refs):

```powershell
git fetch --no-tags https://github.com/has207/xenia-edge.git edge:refs/remotes/research-edge/edge
git fetch --no-tags https://github.com/xenia-canary/xenia-canary.git canary_experimental:refs/remotes/research-canary/canary_experimental
git fetch --no-tags https://github.com/xenia-project/xenia.git master:refs/remotes/research-xenia/master
gh issue list -R xenia-canary/xenia-canary --state open --limit 1000
gh pr list -R xenia-canary/xenia-canary --state open --limit 1000
gh issue list -R has207/xenia-edge --state open --limit 1000
gh pr list -R has207/xenia-edge --state open --limit 1000
```

Use `gh api --paginate` for issue comments, PR files/reviews and updated closed
items; record retrieval time and any API limit. The investigation snapshot
retrieved up to 100 comments/files per selected item; no selected comment thread
was truncated. Refresh rather than treating the snapshot as permanent upstream
status. The first-parent boundary is not necessarily a project's first original
fork; retained author dates can predate rewritten history.

## RG-FIX-006: static CRT non-local jumps (2026-09-28)

Original adaptation of the SDK's existing manual host-jump support at
`8e4b9f10c502a7696058b5522923701a11e7b27e`; no upstream patch ported.
[Investigation and regression record](crt-nonlocal-jumps.md) pins the reference
and separates recognition, synthetic execution and title evidence.
Read-only Edge `12e3b4223dd4c2e41d57ea4b4477546affe4ce10` uses native Windows
unwinding for JIT/thread reentry; classification D/C, not imported. No upstream
PR/merge or regression status is asserted. The shared static implementation is
tracked by [SDK #107](https://github.com/furqanagwan/rexglue-sdk/issues/107) and
[ADR-010](adr/ADR-010-static-nonlocal-jumps.md).

## RG-GDK-040: cross-buffer XMA sub-streams (2026-09-29)

Classification B, shared compatibility adaptation: Canary
[PR #983](https://github.com/xenia-canary/xenia-canary/pull/983), merged
2026-05-14 as `b575c684187d6a77a91cb8ff3297173326206a58`, head
`c77d274464a971b8c47ad53527ee43f694a2433d`; compared at Edge reference
`12e3b4223dd4c2e41d57ea4b4477546affe4ce10`. Keep the target sub-stream's
packet-index remainder across input buffers. The earlier RG-GDK-018 audit had
deferred this path without a two-buffer fixture. Legends captures and real
FFmpeg synthetic tests now demonstrate the missing behavior. No wholesale
context or codec replacement. The PR's AC6/Afro Samurai improvements and spot
checks are upstream reports, not local title coverage. Split-header, frameless
skip-chain, loop and drain follow-ups remain covered by the existing suite.
Closed-unmerged Edge #236's silence fallback remains rejected. Detailed source,
review comments, limitations, tooling and validation: [RG-GDK-040 record](xma-cross-buffer-streams.md),
[SDK issue #124](https://github.com/furqanagwan/rexglue-sdk/issues/124).

## RG-GDK-041: Xbox guide from the console's XUI (2026-09-30)

Classification A: an original implementation; no Xenia, Canary or Edge code is
involved. Format reference only, with no code or definition files taken:
[SGCSam/XUIHelper](https://github.com/SGCSam/XUIHelper) at
`c0d083036c6b0e3cdec5a3df0abfca0e58973117` (GPL-3.0), for the XUR v8 layout. Behaviour
reference only, for timing cross-checks:
[ZivvoZ/dashx360](https://github.com/ZivvoZ/dashx360) at
`9f58af56be53bba4ca12ba3dad1adb02b542ec21`. Scenes and media come at run time from the
owner's dashboard 2.0.17559 system update, and nothing from it is committed. The
package validated locally has SHA-256
`8119312192ad3ac41345336c6302a97bc471af2ab6689f04ffe92e9e07bc7c45`
(`su20076000_00000000`). Design: [ADR-011](adr/ADR-011-xbox-guide-from-system-xui.md);
record: [Xbox guide](xbox-guide.md); tracking:
[Guide issue #1](https://github.com/furqanagwan/xbox/issues/1)
(transferred from SDK #127).

Source ownership moved on 2026-10-06 to
[xbox-guide](https://github.com/furqanagwan/xbox), pinned by this SDK's
Guide submodule. Extraction source: local SDK
`d1a87b4ef0a09c7a7813ab2a2b27976de01de203`; classification B (source/host
organization, no upstream subsystem import). Source notices and an affected
commit-history record are preserved. `ResolveFile` moves to a separate adapter
translation unit without changing lookup behavior, allowing standalone scene
linkage. The Guide suite matches the baseline; private asset/title/pad checks
remain blocked. [Extraction evidence](xbox-guide-extraction.md) gives the pin,
issue transfers and full validation.

### furqanagwan/xenia-edge fork sync of 2026-09-30 (reviewed 2026-09-30)

Edge `edge` from `12e3b4223dd4c2e41d57ea4b4477546affe4ce10` to
`23e76712f` (35 commits, including a merge of Canary `canary_experimental`),
compared with ReXGlue `main` at `52a9fb1`.

**Ported (RG-GDK-042, #130):** NT file read completion. Classification A, all titles.

- Sources: Edge `8a027ef4f` "Make the file wait event manual reset", `beb3230fe` "Complete posted NtReadFile requests like NT", `8c8c550ae` "Tighten overlapped and scatter read completion", `e60b90ac0` "Fix async NtReadFile regressions" (Split/Second startup crash, Cars loading livelock), `5aa0d3dea` "Ensure XamEnumerate item count is zero on failure"; Herman S., 2026-09-28/29.
- Local evidence: the same code was here. The file event was auto-reset; a read on an asynchronous handle returned PENDING but queued its APC only on success, so a failed read never completed for the caller; status blocks and XAM overlapped results were written status first; `XamEnumerate` wrote an uninitialised count when it failed early.
- Adaptation: ReXGlue completes reads inline (Edge posts them to a worker), so only the completion rules are ported: notification event cleared when a request starts; APC for every read reported PENDING; `NtReadFileScatter` APC only for asynchronous handles or success; count, fence, then status everywhere (`WriteIoStatus`, `CompleteOverlappedEx`). The `e60b90ac0` inline completion inside a user APC is not needed: `XThread::DeliverAPCs` keeps delivering until the queue is empty, so a completion routine that chains the next read gets its APC.
- Tests: `kernel_tests [file_read]`; the failed-read APC and second-wait tests fail on the old code. Quantum of Solace (GDK Release) booted, reached the front end and streamed its attract movie, 0 errors.
- Known regressions: none reported upstream after `e60b90ac0`.

**Queued: Canary GPU PRs merged 2026-09-26 to 09-30**, all Classification A unless noted, not yet in ReXGlue:

- xenia-canary #1111 "VIZ_QUERY predication" (merged 2026-09-29; Edge `692cd59cf`): unblocks RG-GDK-010b (#65).
- #1248, #1249, #1252, #1245: ported in RG-GDK-044 (below).
- #1250 "idTech 5 virtual texturing fix; snap fetches to guest texel centers" (Edge `c3cd8617b`): needs Canary #1072, #1137 and #1177 first; RG-GDK-045 (#135).
- #1242 XAM `X_MARKETPLACE_ENTRYPOINT` bounds check and `X_MODULE_FLAGS` logging (Edge `9a0474a2f`), #1139 `XMPGetMediaSource` stub (Edge `1c5059dcc`): low value, taken with the next XAM batch.

**Not ported:**

- Edge `a7c39fa7d`, `1222c23f7`, `16df25981`, `b83724656`, `34387b31f`, `78315ec19` (memexport await split, per-submission waits, resolve/memexport copy-back ordering, resolve output before file reads): all build on Edge's resolve read-watch and host-buffer memexport redesign (`77f2cca80`, `935e03876`), which ReXGlue does not have. **Watch** with that redesign.
- Edge `98b6319c5` (skip re-arming fully armed watch blocks): a speed fix for Edge's read watches; ReXGlue's older `EnableAccessCallbacks` has only invalidation watches and no evidence of the cost. **Watch.**
- Edge `aa3339970` (vblank pacing with `guest_time_scalar`): ReXGlue's `VblankPacer` already sleeps to the deadline, and the guest time scalar is fixed at 1.
- Edge `50d999623`, `80d9a5c58` (guest scheduler timer waits, cooperative waiter boost): Edge's cooperative guest scheduler, which ReXGlue does not have.
- Edge `a0d11bec9`, `f453ede2e`, `f2583e9de` (JIT code cache, JIT precompile of address-only functions, JIT scanner extents): CPU JIT, not applicable. Static codegen already finds data-referenced leaves (RG-FIX-002 work, PR #105).
- Edge `89b7047dd` (SDL XInput pass-through), `26e1e0fee` (POSIX), `5f3e28236` (macOS CI), `3171a10aa` (Mesa DXIL bit): removed platforms, SDL and the unported DXIL pipeline.

## RG-GDK-044: Canary GPU batch of 2026-09-26 to 09-29 (2026-10-01)

xenia-canary `canary_experimental`, reviewed at `44f5b4a`; tracking
[SDK issue #134](https://github.com/furqanagwan/rexglue-sdk/issues/134). All by
goldislead (boma). No follow-up commits or regression reports against these
four as of 2026-10-01. Ported by formatting Canary's before and after with our
`.clang-format` and applying the difference, except where noted.

| Canary PR | Commits (merged) | Class | Adaptation | Tests |
| --- | --- | --- | --- | --- |
| #1248 `RB_COPY_SURFACE_SLICE` for volume resolve slice spacing | `d8731edc99ecc438eb4cd1a8754341d7396a061e` (2026-09-26) | A | Hand port: our `GetResolveInfo` predates Canary's `texture_address` helpers, so the slice height (`RB_COPY_SURFACE_SLICE / copy_dest_pitch` for array destinations, else `copy_dest_height`) feeds `height_aligned_div_32` and the three 3D address calls. `xenos.h` comment updated. | Existing gpu resolve fixtures (2D unchanged). No local volume-resolve fixture or title. |
| #1249 "More guest texture layout bits" | `ace153cb84704cea5634a056076585b570380a26`, `81e3deaee2dbd5d3125f87da2194a61e418851ce`, `c332733afd14ed3aeb08b38d0cabffa58d1c8c2f` (2026-09-27) | A | Hand port into `texture/util.cpp`: level 0 packed tails take mips from the base address; packed base slice strides use power-of-two height and depth; the 3D upper bound searches each 8-block run's last block instead of the closed form, which reached 0x880 and a page past the last block. | `unit_tests [texture_layout]`: packed level 0 mip page, volume slice stride (16 slices, not 12), and the 3D upper bound equal to the last block's end for boxes from the origin (bpp 1 to 16, pitch 32 to 128, widths past the pitch). |
| #1252 "Clamp stacked-texture layer index for Inf/NaN coords" | `04085efaafcfb8907749f200514c21433db2ebeb` (2026-09-29) | B (584107FB black backdrop) | Applied as is to `dxbc_translator_fetch.cpp`. SPIR-V half not applicable. | Build and the gpu suite; no local stacked-texture title. |
| #1245 "experimental round-toward-zero cvar for mulsc" | `3390fc219b32be4a87777ddd72217259b24dc09e` (2026-09-29) | D (opt-in; 5451080D, 4B4D07F6, 5451086D) | DXBC (Veltkamp/Dekker error) and interpreter as is; cvar `mulsc_round_toward_zero` (GPU/Shader, restart) beside `gpu_scalar_approximation_rounding`. SPIR-V half not applicable. | `gpu_tests [alu]` "MULSC rounds to nearest, or toward zero when asked": bit-exact against round-to-nearest with the option off and toward zero with it on (products of 1/3 that round away included). |

Known regressions: none reported upstream. #1250 is split out to RG-GDK-045
([#135](https://github.com/furqanagwan/rexglue-sdk/issues/135)): it builds on
Canary #1072 (`d119505289`), #1137 (`6a45452087`) and #1177 (`0c843efb32`),
and #1072 has an open D3D12 device-loss report (Canary #1134).

## RG-GDK-053: PPC instruction gaps (2026-10-01, in progress)

Tracking [SDK issue #149](https://github.com/furqanagwan/rexglue-sdk/issues/149).
The 007 generated binaries contain no stubs for these instructions; no title
compatibility improvement has been demonstrated. Upstream regression status
for these CPU changes is unknown. These are adaptations to static C++ codegen,
not ports of the Edge JIT.

| Reference | Class and motivation | Local adaptation | Evidence |
| --- | --- | --- | --- |
| has207/xenia-edge [`10da45ea4073d0a2ca82dfcf25c57d83ea3d879f`](https://github.com/has207/xenia-edge/commit/10da45ea4073d0a2ca82dfcf25c57d83ea3d879f), 2026-10-01, "Implement XER[OV] and XER[SO] for the PPC OE forms" | A, arithmetic correctness; sticky SO and 64-bit overflow | OE add, subtract, negate, multiply and divide dispatch and XER test directives; OE doubleword record forms compare all 64 bits | `instr_overflow.s` plus pinned Edge `instr_addo.s`, `instr_divwo.s`, `instr_mcrxr.s`, `instr_mullwo.s`, `instr_nego.s`; `mcrxr` mnemonic encoded as `.long` for the bundled assembler |
| xenia-canary [`72ce1309715b30d07035c8511dffc13c0b92a2d9`](https://github.com/xenia-canary/xenia-canary/commit/72ce1309715b30d07035c8511dffc13c0b92a2d9), 2026-06-14, "Xbox360 cache line is always 128 bytes" | B, fixes 4E4D07E0 and 4E4D083D | Align `dcbz` to a 128-byte block and clear 128 bytes as `dcbzl` already does | `instr_dcbz.s` checks whole block and adjacent sentinel |
| has207/xenia-edge PPC corpus at [`b5cc59e854`](https://github.com/has207/xenia-edge/tree/b5cc59e854/src/xenia/cpu/ppc/testing), reviewed 2026-10-01 | A, missing VMX unsigned-word carry, average and maximum forms, and VMX128 shift-table aliases | Four static vector builders with lane-wise unsigned arithmetic; `lvsl128` and `lvsr128` reuse existing shift-table builders | `instr_vector_gap.s` plus pinned Edge `instr_vaddcuw.s`, `instr_vsubcuw.s`, `instr_vavguw.s`, `instr_vmaxuw.s`, `instr_lvsl128.s` and `instr_lvsr128.s` |
| has207/xenia-edge `vpkpx` implementation and [`instr__gen_vpkpx.s`](https://github.com/has207/xenia-edge/blob/b5cc59e854/src/xenia/cpu/ppc/testing/instr__gen_vpkpx.s) at `b5cc59e854`, reviewed 2026-10-01 | A, missing pixel pack opcode | Snapshot both source vectors, then pack the specified alpha bit and RGB high five bits per pixel | Pinned Edge `instr_vpkpx.s` and `instr_vpkpx_small.s` (destination/source alias case); full generated corpus not yet imported |
| has207/xenia-edge PPC corpus at [`b5cc59e854`](https://github.com/has207/xenia-edge/tree/b5cc59e854/src/xenia/cpu/ppc/testing), reviewed 2026-10-01 | A, VSCR NJ and SAT state | Context carries NJ through guest thread save/restore; `mtvscr` changes the vector FP flush mode and resets codegen's CSR state; saturating vector arithmetic and packs set sticky SAT | Pinned `instr_mfvscr.s`, `instr_seq_njm.s`; `instr_vscr_sat.s` checks sticky and reset behavior |
| has207/xenia-edge [`f4af1e2a77703ca60a4dae512fb047cfe73ecb11`](https://github.com/has207/xenia-edge/commit/f4af1e2a77703ca60a4dae512fb047cfe73ecb11), 2026-02-13, "Implement and fix FPSCR-related instructions" | A, missing `mcrfs` | Track guest FPSCR bits separately from the host rounding control word; move and clear a selected field; fix `mtfsf` field mask direction | Pinned `instr_mcrfs.s` plus `instr_fpscr_state.s` with CR and preserved-rounding assertions |

Other scalar and system dispatch additions in this issue use existing static
builders or narrow no-op implementations; pinned Edge `instr_icbi.s`,
`instr_isync.s`, `instr_mfmsr.s`, `instr_mtmsr.s` and `instr_mtmsrd.s` cover them.
Their title and vendor regressions have not yet been measured. Scalar
arithmetic-produced FPSCR causes and derived summary bits are now tracked for
add, subtract, multiply, divide, fused multiply-add/subtract and square-root
families, including their single-precision forms. The adaptation uses host fenv
for OX/UX/ZX/XX and explicitly classifies PowerPC invalid subcauses that host
fenv does not distinguish. Sticky FX, enabled FEX, invalid VX and record-form
CR1 are checked by unit tests and `tests/ppc/asm/instr_fpscr_arithmetic.s`.
`fcmpu` records VXSNAN for signaling NaNs; `fcmpo` records VXVC for quiet NaNs
and for signaling NaNs when VE is disabled, following IBM's [fcmpu] and [fcmpo]
descriptions. The 2026-10-08 [arithmetic FPSCR follow-up](#rg-gdk-053-arithmetic-fpscr-follow-up-2026-10-08)
adds FPCC/FPRF result classification and partial conversion/estimate exception
tracking; later follow-ups add FR/FI and estimate result classification.
The broader Edge corpus, title validation and release gates remain open.

[fcmpu]: https://www.ibm.com/docs/en/aix/7.2.0?topic=set-fcmpu-floating-compare-unordered-instruction
[fcmpo]: https://www.ibm.com/docs/ssw_aix_71/assembler/idalangref_fcmpo_instrs.html

## RG-GDK-048: achievement enumerator offset (2026-10-01)

xenia-canary PR #861 "[XAM] Fixed enumeration of achievements once again",
commit `603355ae5bf39bb08e5f2815bb37406af83ea6d8` (Gliniak, 2026-01-27, merged
2026-01-29). Class B (007 Legends `415608D8`). No follow-up regressions found
in Canary's later `xenumerator` history as of 2026-10-01.

- Found running 007 Legends: it pages through its achievements with one
  `XamUserCreateAchievementEnumerator` per page (offset 0, 25, 50). ReXGlue
  ignored the offset, so every page was the first 25. The title kept paging
  about every 130 ms after a save, appending each page to a list of 56-byte
  entries, until its allocator returned null and it wrote through it
  (`sub_826D7808`, after about 2,700 pages, twice).
- Adaptation: `XStaticAchievementEnumerator` starts at the offset, as Canary's
  `XAchievementEnumerator` does, and returns `X_ERROR_NO_MORE_FILES` for an
  offset at or past the end (Canary subtracts first and would underflow).
- Also added: debug logs for each `XamEnumerate` (handle, item size, items,
  result) and `XamContentCreateDeviceEnumerator`.
- Tests: `kernel_tests` "Achievement enumerators page from the title's offset"
  (60 achievements: 25 from 1, 25 from 26, 10 from 51, then none, and none
  past the end). 007 Legends after the fix: pages of 25, 25, then
  `NO_MORE_FILES`.

## RG-GDK-069: two Canary defaults, and title cvar defaults (2026-10-02)

Found with NHL Legacy Edition (`454109EC`), whose matches drew black but for
the HUD ([furqanagwan/nhl](https://github.com/furqanagwan/nhl) RG-NHL-003).

| Canary commit | Class | Adaptation | Tests |
| --- | --- | --- | --- |
| `d36b1b38304d58da3daadff76aba0da9bd37b278` "[GPU] gpu_allow_invalid_fetch_constants true by default" (disjtqz, 2023-10-12) | A (Edge has the same default) | Default flipped in `graphics/flags.cpp`. NHL logged 26,635 "invalid type" texture fetch constants in four minutes, skipping those draws. | NHL: no fetch constant warnings with it on. |
| `4452e300acfc0ceb76fab579c12915d70133ce92` "[Emulator] Changed default config values" (Gliniak, 2024-08-31), `mount_cache` only | B (EA titles) | `mount_cache` (Runtime, default on) mounts `cache:`, `cache0:` and `cache1:` as host folders under `<cache root>\partitions`, the prefixed devices first, as Canary's `xenia_main.cc` does. Replaces the inherited note that `cache:` should fail cleanly. | NHL keeps its cache files there (`809284.ver`, `highlights.sav`); the three 007 titles boot and reach their menus with it. |

Neither fixed NHL's black matches: the scene draws on the ROV render target
path and not on the RTV path (tested with occlusion queries faked, 2x MSAA
off, float24 depth rounding and conversion, gamma as unorm16 off and stencil
output off on RTV, none of which drew it). The RTV cause is open; NHL opts
into ROV through the new title cvar defaults (`rexglue_configure_target`
`CVAR_DEFAULTS`, `rex::cvar::SetTitleDefault`, ADR-009).

## RG-GDK-054: Edge's PPC test corpus (2026-10-02)

| Source | Class | Adaptation | Tests |
| --- | --- | --- | --- |
| has207/xenia-edge `b5cc59e854020f8406c0b3a96eff6a3d036a514b` (2026-10-01), `src/xenia/cpu/ppc/testing/*.s` and `skip.txt` | A (test data) | Copied unchanged into `tests/ppc/corpus` (578 files; xenia-project `gen_tests` hardware captures plus hand-written cases). 11 files the bundled binutils can't assemble are listed in `assembler_unsupported.txt`. | `ppc_corpus.*` CTest tests (`REXGLUE_PPC_CORPUS=ON`). |
| Edge `e25eaffff` (bracketed `MEMORY_IN`/`MEMORY_OUT` bytes) and its runner's per-test clearing of `0x10001000`-`0x10010000` | A (harness) | Reimplemented in `rexglue recompile-tests`: bracketed bytes and scalars, `xer` in/out, per-file function names, `skip.txt` labels without `test_`. The new table runner also resets the host FPSCR per case. | The corpus runs deterministically (two runs, same 30,967 failures). |

ReXGlue's generator emits the corpus as data (`--table`, `ppc_table_runner.h`)
with one Catch2 test per file: a test per case gave 48 sources of about 2 MB
whose compile exhausted memory. Failures are recorded with their cause issue
(#149, #151, #185) in `tests/ppc/corpus/known_failures.txt`
([regression strategy](regression-strategy.md#ppc-test-corpus-rg-gdk-054)).

## RG-GDK-055: PowerPC floating-point rules (2026-10-02)

has207/xenia-edge commits (August-September 2026, all class A: correctness,
measured against the hardware-captured corpus). Edge makes them in its HIR
frontend and JIT backends; ReXGlue puts them in inline helpers in
`include/rex/ppc/fp.h`, which the codegen builders call, and a table in
`src/system/ppc_fp.cpp`.

| Edge commit | Rule | ReXGlue |
| --- | --- | --- |
| `9804846f4`, `19fb3979d`, `01efb80ed` | Multiply-add family: a NaN is picked in A, B, C order, quieted, and the negated forms don't negate it | `madd`/`msub`/`nmadd`/`nmsub` (and `s` forms), `vmadd`, `vnmsub` |
| `cf43c4c52`, `6de9c21ec` | Invalid operations give the positive default QNaN | `nan_result`; `vnan` for VMX elements |
| `28f38affe`, `8b19ee756`, `078a07b53` | Single precision answers the default QNaN for a double-denormal operand (all finite); `fdivs`/`fsqrts` instead skip rounding; one cheap screen per op | `single_denormal`, one unlikely branch per single op (`Suspect`/`SingleSlow`) |
| `de4d24493`, `32920009d` | Record forms set CR1 (FX, FEX, VX, OX) from the host exception status, plus invalid the host doesn't report | `recorded`, `madd_invalid`; per instruction, not sticky in FPSCR |
| `e4b13738c`, `ed9bfc9a4` (and Canary `7ff152a5a`) | `vcmpbfp` sets both bounds bits for a NaN; `vmaxfp`/`vminfp` quiet a NaN, flush denormals, order +0 above -0 | `vcmpb`, `vmax`, `vmin`; `vcmpbfp.` sets only CR6[2] |
| `378c95215` | `fctiw`/`fctid` saturate, NaN gives the sign-extended minimum; CR1 for the conversions | `to_int32`, `to_int64`, `set_cr1_convert` |
| `fb225d975`, `vrsqrte_table.cc`, x64 `EmitFrsqrteHelper` (all at `b5cc59e854`) | Estimates: `vexptefp`/`vlogefp` polynomials snapped to the 2^-11 grid; `vrsqrtefp` coefficient table; `frsqrte` 16-entry table | `vexpte`, `vloge`, `vrsqrte`, `rsqrte`, `set_cr1_estimate` |
| `c496db01f` | `lfs`/`stfs` keep a signalling NaN signalling | `load_single`, `store_single` (unit tested; not in the corpus) |

`vmaddfp`/`vnmsubfp` are fused, as on hardware: FMA3 through inline assembly
when the CPU has it (the titles build for SSE4.1), otherwise in double. This
changed ReXGlue's own `instr_vmaddfp` expectation by one ulp, to the correctly
rounded value. Known regressions in Edge: none recorded against these commits.

Measured: the corpus loses all 27,585 #151 known failures (and one #185
case, `vmaddfp_1`); PPC 1,473/1,473. A micro-benchmark (`-O2 -msse4.1`,
vectorisation off, three runs) puts `fadds`/`fmuls`/`fmadds` at +20-40% per
op, `fadd` +5%, `vmaddfp` and `vmaxfp` level, `vnmsubfp` +30-60%. Title frame
time is not yet measured.

## RG-GDK-056: host and guest FP modes kept apart (2026-10-02)

has207/xenia-edge commits (September-October 2026, class A: correctness,
latent; no symptom was seen in a title). Edge switches modes in its JIT's host
thunks; ReXGlue does it in the call wrappers, with two scopes in
`include/rex/ppc/context.h`.

| Edge commit | Rule | ReXGlue |
| --- | --- | --- |
| `10c8ae795` "Enter host code with the default MXCSR" (2026-10-01) | Host code called from guest code runs at round to nearest, no flush | `HostFpScope` in `HostToGuestFunction` (every `REX_HOOK`/`REX_EXPORT`) |
| `95b14f55f` "Keep host and guest FP modes apart around guest callbacks" (2026-10-01) | Guest code entered from host code finds its own mode | `GuestFpScope` in `GuestToHostFunction`, `ImportFunction` and `FunctionDispatcher::Execute` (thread start, APCs, interrupts) |
| `efbac5e9f` "Run exception handlers in the host FP mode" (2026-10-01) | Host exception handlers don't inherit the faulting guest mode | `exception_handler_win.cpp` clears the guest bits around the handlers |
| `cb86a688e`, `2802ae523` (mode tracking after host calls) | The JIT forgets its tracked mode after a host call | Already so: codegen resets its tracked state after calls. `HostFpScope` restores the guest bits from `ctx.fpscr`, so a callback that changed the rounding mode is kept and the cache stays true |

Not covered: `REX_HOOK_RAW` hooks and stubs run in the caller's mode (they get
the raw context; a raw hook doing float math should open a `HostFpScope`).
Cost, measured on the development laptop: about 9 ns per export call while the
guest is in flush mode (two control-register writes), 1-2 ns otherwise.
Tested by `tests/unit/ppc/fp_test.cpp` ("Host code runs in the host FP mode").

## RG-GDK-045: Canary fixed-format texture fetches (2026-10-03)

The texture fetch side of four xenia-canary PRs, taken as they stand at
Canary `6260a87b8551d2f5ffb30763573d84652bb9e00e` (2026-10-02) rather than
replayed one by one:

| Canary | Class | What | ReXGlue |
| --- | --- | --- | --- |
| #1072 `d119505289` (2026-07-01), fetch half; `2ddc5ef737` (2026-07-28, unsigned-biased scaling) | A (compatibility: Canary reports black screens fixed in dozens of titles) | Integer `num_format` on fixed formats scales the normalized host sample back to the guest's integer range; unsigned-biased decodes as offset binary | `FormatInfo::component_bits`/`fixed` (`info_formats.cpp`), `texture_util::GetIntegerScaleBits`, `texture_integer_scale_bits` system constant (`xenos_draw.hlsli` too; tessellation bytecode rebuilt), DXBC fetch block |
| #1137 `6a45452087` | A | Walk the guest swizzle to each output's source component (past the stored ones, the last one) | `GetIntegerScaleBits` |
| #1177 `0c843efb32` (2026-08-26) | A (SSAO in 4D5309C9, 4D530AA4) | Normalized unsigned fixed fetches round to 16 fractional bits; point sampled 4-7 bit components rebuild the guest's `n * (2^w + 1) / 2^(2w)` | bit 24 and the widths; DXBC fetch block |
| #1250 `c3cd8617b1` (2026-09-27) | B (idTech 5 virtual texturing, 425307EC seams) | Point sampled 2D fetches snap to the texel center instead of adding the coordinate epsilon | bit 26, `CanSnapToTexelCenter`, `kTextureCoordEpsilon` |

Not taken: the resolve half of #1072 (8_8_8_8_GAMMA PWL decode before MSAA
averaging, `copy_dest_number` packing) and its follow-ups, split to
RG-GDK-073 (#189). Known regression: Canary #1134 (open), D3D12 device loss on
a GTX 1660 SUPER in EA titles, bisected to `d119505` with both halves; the
title runs on NVIDIA here are part of the deferred game-run batch. AMD and
Intel not run. Tests: `tests/unit/graphics/fetch_conversion_test.cpp`
(packing for narrow formats, both num_formats, swizzle walk, gamma, point
flag); gpu suite 43/43; shader bytecode reproducible.

## RG-GDK-067: per-title replacement shaders (2026-10-03)

No Xenia counterpart (class H: new ReXGlue feature). Modelled on the named
replacement shaders in Microsoft's Xbox One/PC backward compatibility GPU
emulator (`VGPUDX12.dll`, observed in the installed PC BC files; nothing
copied). `ShaderReplacements` (rexcore) reads `<HASH>[_<MOD>].<stage>.dxbc`;
the D3D12 pipeline cache swaps a translation's binary after translating it,
keeping its bindings. Build side: `rexglue_configure_target(SHADER_REPLACEMENTS)`.
See [shader replacements](shader-replacements.md). The QoS frame-capture check
the issue asks for is in the deferred game-run batch.

## RG-GDK-066: runtime indirect trace and fallback build (2026-10-03)

No Xenia counterpart (class H: ReXGlue tooling). Design observations from the
build metadata of Microsoft's PC backward compatibility modules
(`xeo3_<hash>.dll` and `_no.dll`, `ficompiler` control files); no Microsoft
code or data used. `--indirect_trace` records unregistered indirect targets as
a codegen `[functions]` include; `REXGLUE_RECOMP_FALLBACK` builds matching
generated files unoptimised. See
[indirect function discovery](indirect-function-discovery.md#runtime-trace-and-the-fallback-build-rg-gdk-066).
The QoS trace round trip is in the deferred game-run batch.

## RG-GDK-062/063: selective upscaling and MSAA boost design (2026-10-03)

No Xenia counterpart (class H). From the launch arguments and symbols of
Microsoft's PC backward compatibility GPU emulator (`scalingResolutions`,
`aaBoostOn`, `aaBoostTargetMsaa`; nothing of theirs used). Design in
[ADR-012](adr/ADR-012-selective-upscaling-and-msaa-boost.md); phase 1 adds
`resolution_scale_targets` parsing and matching (`ScalingResolutionList`,
`tests/unit/graphics/scaling_list_test.cpp`) and the
`log_resolution_scale_targets` report, with no rendering change.

RG-GDK-062 phase 2 (2026-10-03): resolves the list doesn't name are written at
the guest's size through the existing resolve downscale shader (xenia-canary
`a635ac64f`, already ported for readback), now also dispatched into shared
memory (`DispatchResolveDownscale`), with `MarkRangeAsNativeResolved` in the
texture cache; resolve readback reads shared memory for such ranges. ADR-012
revised accordingly.

RG-GDK-063 (2026-10-03): `resolve_downscale_average` averages native resolves
per byte (a third mode of the resolve downscale shader; owner's choice over
true MSAA boost) and `resolution_scale_targets=none` makes every resolve
native, together supersampling at the guest's size. ADR-012 revised.

## RG-GDK-010b: VIZ_QUERY predication (2026-10-03)

Port of xenia-canary #1111 (merged as `692cd59cf`, 2026-09-29; Edge took the
same commit; class C/H) for [#65](https://github.com/furqanagwan/rexglue-sdk/issues/65).
Behind `occlusion_query_viz`, default off and requiring a restart, as in Canary.
Survey draws (`PA_SC_VIZ_QUERY` enabled with `kill_pix_post_hi_z`) run as
depth-only occlusion query segments shared with the ZPD report machinery
(native queries on the RTV path, the ZPass counter on ROV); draws carrying a VIZ
token run under D3D12 `SetPredication` (`EQUAL_ZERO`) from a 64-entry predicate
buffer, or are culled on the CPU once the answer is resolved. Any unmeasured
survey (copy mode, kills or alpha to coverage, pipeline still compiling, lost
segment, exhausted pool) makes its ID visible, and memexport or copy-mode
consumers are never skipped. Adaptation: ReXGlue names (`OpenQuerySegment`,
`CloseQuerySegment`, `UpdateZPDSegment`), reports never count survey draws, and
the `PA_SC_VIZ_QUERY_STATUS` bits still read back as visible. Behaviour change
with the cvar on: non-survey draws with the kill bit are dropped, as on
hardware. Pipeline cache description version bumped (`viz_survey` bit).
Tests: `tests/gpu/viz_fixture_test.cpp` (hidden survey skips its consumer,
visible keeps it, both with the consumer in the same submission under the
predicate and after a flush using the resolved answer, RTV and ROV; a survey
nothing rejects stays visible). Known regressions: none reported upstream at
the pin; title validation is in the deferred game-run batch.

## Texture-typed vertex fetch constants (2026-10-03)

xenia-canary `b083312b8` ("[D3D12] Don't drop kInvalidVertex draws",
2026-10-02; class B, correctness and compatibility; fixes missing character
models in 5454086C and 425607FE). ReXGlue already allowed `kInvalidVertex`
under `gpu_allow_invalid_fetch_constants` (default on); the commit also lets
texture-typed vertex fetch constants through, which this adopts in
`D3D12CommandProcessor::IssueDraw`. Tests: `tests/gpu/invalid_fetch_fixture_test.cpp`
(drawn with the cvar on, dropped with it off; `DrawOptions::fetch_type` in the
guest draw helpers). Known regressions: none at the pin.

## RG-GDK-072: PPC corpus wrong results (2026-10-03)

Class A (correctness), from Xbox 360 hardware results in xenia-edge's PPC
corpus ([#185](https://github.com/furqanagwan/rexglue-sdk/issues/185)).
Neither Edge (`504cbee7eb`, the latest change to its `ppc_emit_altivec.cc`)
nor Canary handles any of them: both shift `vsl` by one count, return the
same dot product sign and have no `stwcx.` address check.

- `vsl`: each byte shifts by its own count (`rex::ppc::simde_mm_vsl`); the
  128-bit shift stays as the fast path when all counts agree.
- `vmsum3fp128`/`vmsum4fp128`: a sum that flushes to zero keeps the sign of
  the unflushed sum (`rex::ppc::simde_mm_vmsumfp`); other results unchanged.
- `mffs`: returns the FPSCR bits the guest last wrote with `mtfsf`, with RN
  from the host. Status bits are still not tracked.
- `stwcx.`/`stdcx.` design note: `lwarx`/`ldarx` record the reserved address
  (`PPCContext::reserved_address`, or a function local with
  `reserved_as_local`, as the reserved value already is). The store
  conditional fails without storing unless that address matches, and always
  drops the reservation. The compare-and-swap against the reserved value is
  unchanged, so another thread's store still fails it. A reservation is exact
  to the address rather than the 128-byte granule, since the swap compares the
  reserved value; a title pairing a `lwarx` with a `stwcx.` to another address
  in the same granule would now fail where hardware may succeed. Nothing seen
  does that. Lock loops that pair them correctly are unaffected.

Tests: the 40 `#185` entries are gone from `tests/ppc/corpus/known_failures.txt`
(all 567 corpus files pass); `tests/ppc` and unit tests pass. Title validation
is in the deferred game-run batch.

## RG-GDK-073: 8_8_8_8_GAMMA resolves and copy_dest_number (2026-10-03)

The resolve half of xenia-canary #1072 (`d119505289`, 2026-07-01) with its
follow-ups `2ddc5ef737` (2026-07-28, every destination gets linear values, the
re-encode removed), `fc48d37cdc` (2026-08-10, the cvars removed: the decode and
the number format check always apply) and `2b3f0cb456` (2026-08-05, k_8 with
LOW_BLUE selects alpha), for [#189](https://github.com/furqanagwan/rexglue-sdk/issues/189).
Class A/B (correctness; Canary reports less blowout in at least four titles and
fixed composites in 5451080D and 4D530808).

- `resolve.xesli`: 8_8_8_8_GAMMA sources decode the PWL curve (RGB only) per
  sample before MSAA averaging and exponent bias; k_8 destinations with the
  swap bit read alpha. Hand-ported: ReXGlue's resolve loads use the uint
  vector buffer and pixel-index addressing, not Canary's byte buffer.
  Canary's always-on `decode_pwl_gamma` bit is left out; the shader checks the
  format.
- `pixel_formats.xesli`, `resolve_full_*.xesli`: fixed packs take the
  destination number format (`XePackFixed`).
- `draw_util::ResolveInfo::GetCopyShader`: gamma sources and destinations
  whose number format isn't the EDRAM's own take the full resolve.
- Bytecode rebuilt with `scripts/build_shaders.py` (the ten full resolve
  shaders change).
- Not taken: `437a7280cf` (Canary #1163, EDRAM single-sample addressing),
  which touches the same shaders but is a separate render target cache change
  (RG-GDK-009 row above).

Tests: `tests/gpu/resolve_gamma_fixture_test.cpp` (8_8_8_8_GAMMA at 1x and
averaged 4x into 8_8_8_8 and 2_10_10_10 on RTV and ROV; 8_8_8_8 into signed
and unsigned integer destinations). Known regression: Canary #1134 (open),
D3D12 device loss on a GTX 1660 SUPER in EA titles bisected to `d119505`;
the FIFA Street and 007 NVIDIA runs are in the deferred game-run batch, and
AMD and Intel are not run.

## Resolution-scaled lines as quads (2026-10-03)

has207/xenia-edge `7d0a45263` (2026-09-30, "[GPU] Expand resolution-scaled
lines to 1 guest pixel wide"; class A, correctness at resolution scales above
1; Dragon's Dogma's 1024x8 tone-mapping LUT came out mostly empty at 3x).
Edge did it in its SPIR-V built-in geometry shader; ReXGlue keeps DXBC, so
this is a DXBC port of the same algorithm into
`PipelineCache::CreateDxbcGeometryShader` (`PipelineGeometryShader::kLineList`):
each segment of a line list or strip becomes a quad half a guest pixel either
side of the line, measured in screen space through the point constants'
NDC size of a guest pixel, which the command processor now also sets for
line draws. Zero-length and NaN lines are dropped. Selected only when the
draw resolution scale is above 1; the pipeline description's and geometry
shader key's `geometry_shader` field gained a bit (description version
bumped). Noted on epic [#53](https://github.com/furqanagwan/rexglue-sdk/issues/53),
whose DXIL path would carry Edge's version. Tests:
`tests/gpu/line_scale_fixture_test.cpp` (RTV and ROV, three line positions:
one guest pixel of coverage at 1x and 2x; half without the expansion, checked
by disabling it). Known regressions: none at the pin.

## RG-GDK-032 stage 1: DXIL toolchain build (2026-10-03)

has207/xenia-edge `edge` at `0788c561e3` (2026-10-03): the Mesa
`spirv_to_dxil` build recipe from `third_party/CMakeLists.txt` (meson out of
tree, release `/MD`, static archives driven by ninja target, the same meson
option set) and its Mesa pin, has207/mesa `7a1fc756809f3bdc9771b54e6036b156389dfc85`
("microsoft/compiler: use c99_alloca.h instead of malloc.h"); the DXIL signing
approach (DXIL.dll `IDxcValidator`, in-place edit, validator version stamped
from `IDxcVersionInfo`) from `src/xenia/gpu/spirv_to_dxil_compiler.cc`.
Class C (enabling, no behaviour change). Adaptation: fetched as a SHA-256-checked
archive instead of a submodule so the default build never clones Mesa;
ReXGlue's clang build links the cl-built archives; the D3D12 Agility SDK
(1.618.5) and DXC (1.8.2502.8) come from NuGet, matching the versions in
Microsoft's Fuzion Frenzy BC package rather than Edge's in-tree
DirectXShaderCompiler. Tests: `tests/dxil/dxil_smoke_test.cpp` (sign and
create a pipeline through the Agility runtime). See
[DXIL shader toolchain](shader-dxil.md). Known regressions: none at the pin;
Edge's pipeline is still changing, so later stages re-pin.

## RG-GDK-032 stage 2 (part 1): SPIR-V translator port (2026-10-03)

has207/xenia-edge `0788c561e3`: `spirv_shader_translator{,_alu,_fetch,_memexport,_rb}`,
`spirv_builder`, `spirv_shader`, `spirv_compatibility.h`,
`spirv_fsi_system_constants`, `spirv_builtin_geometry_shader`,
`spirv_to_dxil_compiler`; glslang `a57276bf558f5cf94d3a9854ebdf5a2236849a5a`
(Edge's pin). Class C (enabling; opt-in build only). Also from Edge's
`shader.h`/`shader_translator.{h,cc}`/`xenos.h`: re-entered label register
writes, the call return-point label, point-fetch coordinate registers,
`TextureFetchUsesComputedLod`, `kTexture1DWideMaxRows`. Adaptation: mechanical
rename to ReXGlue conventions; Vulkan device paths removed; `dxil.dll` loaded
from `D3D12\` first. Not taken yet (DXBC-visible parsing changes, with the
wide 1D texture work): 1D fetch XY coordinates (545407D4) and the scalar
second component of three-operand vector ops. Tests:
`tests/dxil/spirv_translator_test.cpp`; the default GPU and unit suites pass
unchanged. Known regressions: none at the pin.

## RG-GDK-032 stage 2 (part 2): D3D12 drawing on the DXIL path (2026-10-04)

has207/xenia-edge `0788c561e3`: `guest_spirv_shader_cache` (SPIR-V
modifications, ported as `GuestSpirvShaderCache`), the Mesa root signature
and `UpdateBindingsMesa` / `GetOrCreateMesaBindlessSamplerIndex` /
`SwitchToNewBindlessSamplerHeap` from `d3d12_command_processor.cc`, and the
translator configuration, geometry shader DXIL and DXIL conversion from
`d3d12/pipeline_cache.cc`. Class C (opt-in). Adaptation: Edge dropped DXBC,
so this runs beside ReXGlue's DXBC path per draw (`gpu_shader_path=dxil`,
fallback for tessellation, DXBC helper pixel shaders, hybrid occlusion
counting, VIZ surveys, ROV and replaced shaders); the host render target
path only; no draw-scale threshold or interpreter placeholder; DXIL pipelines
created synchronously and not stored; separate constant buffer bindings; a
new [shared SRV, shared UAV] bindless pair; the sampler heap switch also
invalidates the DXBC descriptor indices. Tests: CTest `gpu.dxil_parity` (all
52 GPU fixture cases with the DXIL path, strict). Known regressions: none at
the pin; title scenes not yet compared.

## RG-GDK-032 stage 2 (part 3): tessellation, ROV and helper pixel shaders on DXIL (2026-10-04)

has207/xenia-edge `0788c561e3`: `ConvertGuestMesaTessellationToDxil` and
`GetMesaTessHostSpirv`, the Mesa ROV depth-only / VIZ survey and depth-only
pixel shaders and their selection in `CreateD3D12Pipeline` from
`d3d12/pipeline_cache.cc`; the tessellation and FSI parts of
`UpdateBindingsMesa`; the host tessellation GLSL
(`tessellation_{indexed,adaptive}.vs.glsl`, the ten `*.hs.glsl`,
`xenos_draw.glsli`, last changed in Edge `68ca6429f2` and `e3d9428071`),
built as Edge's `xenia-shader-cc` does (glslang, Vulkan 1.0, SPIR-V 1.0,
without its spirv-opt pass). Class C (opt-in). Adaptation: the GLSL is
compiled at build time with the pinned glslang's standalone compiler
instead of a host tool; linked tessellation DXIL is cached per domain shader
modification and made at pipeline creation; float24 helper shaders are
translator-built DXIL (Edge binds its precompiled ones); the ROV guest sample
count rides in the pixel shader modification of PS-less DXIL descriptions
instead of a new description field; hybrid occlusion counting stays on DXBC.
Tests: `tests/gpu/tessellation_fixture_test.cpp` (DXBC and, through
`gpu.dxil_parity`, DXIL; RTV and ROV), the ROV ZPD and VIZ fixtures in strict
DXIL. Known regressions: none at the pin; no tessellating or ROV title scene
compared yet.

## RG-GDK-058: checked sources and media recovery (2026-10-07)

Source: this implementation fork, base `799a5ea`, topic `pc-game-source`;
[SDK #154](https://github.com/furqanagwan/rexglue-sdk/issues/154). Original SDK
work, not an upstream port; no upstream PR head/merge state applies. Class A
for XDVDFS range/cycle checks and read-error handling, class B for source/host
integration. Existing reader notices and candidate partition locations are
retained. The fingerprint is XXH3-128, not authentication. No guest JIT,
graphics algorithm or restricted asset is introduced.

Architecture: [ADR-014](adr/ADR-014-game-source-and-media-recovery.md).
Primary references are Microsoft
[file buffering](https://learn.microsoft.com/en-us/windows/win32/fileio/file-buffering)
and [CD-ROM geometry](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddcdrm/ni-ntddcdrm-ioctl_cdrom_get_drive_geometry_ex).
Tests cover malformed synthetic images, sector alignment, image truncation,
still-open handles, concurrent/rejected retry, UI-thread recovery avoidance,
shutdown cancellation, extraction ownership, generated original/TU identity,
runtime aliases and first-run/controller UI. Results belong in
[release evidence](release-evidence.md). Known new regressions: none in tested
synthetic cases. Physical drive/removal, controller interaction, real-title
launch, friendly mismatch names and console message-box/Active Downloads gates
remain unverified; #154 stays open. The pre-existing Debug tessellation fixture
failure is unchanged.

## RG-GDK-058: standalone host scenes and source adapter (2026-10-07)

Source: original Guide component work, base `3d5ea575a0f6a6ceb9f0c39c097d5c06c59b72aa`,
head `db55a4d`; [Guide PR #8](https://github.com/furqanagwan/xbox/pull/8)
is open/draft, not merged. SDK adapter base `98f006b`, topic
`pc-guide-source-scenes`. Class B for neutral message-box/activity models and
host source/progress/history integration; class A for reading terminal
TitleUpdateJob state before non-atomic errors and using the displayed activity
snapshot for cancellation. No upstream subsystem was copied.

Context-bearing scene models own their contexts and prohibit value moves;
hosts retain private documents and supply resource/input/action callbacks.
SDK source/disc services stay in the SDK. GuideHost accepts immutable host
activity snapshots; existing title-update jobs remain visible and completed
source copies retain history. Media prompts publish XAM system-UI state,
consume held input and use existing termination semantics. Polling enables
ImGui navigation activation on the first controller A press without needing
a prior direction change; custom hit targets explicitly enable navigation.

Tests: standalone Debug/Release, private installed-reference skin controls,
controller initial activation, synthetic ISO extraction/history and affected
SDK UI/source/TU/disc suites in Debug, Release and GDK Release. Full SDK and
installed-consumer results are in [release evidence](release-evidence.md).
Known tested regressions: none after resolving navigation/fixture issues
before publication; the pre-existing Debug GPU assertion remains. Painted
controller/title/media-removal gates and friendly mismatch names remain open.
No Microsoft assets or installed game service identity are included.


## Explicit title Guide selection and empty activity preparation, 2026-10-07

SDK branch `guide-by-title-platform` consumes Guide commit
`5e75b86` on `explicit-guide-presentation`, based on SDK `8cfb449` and Guide
`db55a4d347c9edc74a17632fd7f71311ea1e7410`. Class A: avoid automatic BC scene/source
selection for a 360 title, and hide reused notification controls even when the
initial Active Downloads row count is zero. No upstream port or Microsoft code
is incorporated. The issue boundary remains [Guide #1](https://github.com/furqanagwan/xbox/issues/1).

The owner explicitly requires the prior 360 Guide for 360 titles, and BC scenes
for original Xbox presentation. The SDK supplies per-target selection and shared
host services; the Guide owns typed scene loading and template preparation.
Actual QoS captures exposed the empty activity regression, which was confirmed
and repaired through shared preparation rather than a title-specific workaround.
Both asset sets, standard/GDK Debug/Release, standalone builds and installed
consumer configurations are tested. Exact counts, title rebuilds, private capture
locations and the initial test orchestration/file-lock failure are recorded in
[release evidence](release-evidence.md#guide-selection-by-title-host-2026-10-07).
Original Xbox execution and 1:1 online/visual parity remain unestablished.

## RG-GDK-053: main integration and corpus follow-up, 2026-10-07

PR #162 is updated from its `91ffc711` head against SDK main
`5dad97248d70126c901d2425ce942e5dc9694a28`, preserving main's guest FPSCR
storage, FP helpers, reservations and table-driven test generator. The existing
`b5cc59e854020f8406c0b3a96eff6a3d036a514b` corpus exposes additional `lmw` and
VMX128 cache-hint aliases. These use the existing memory/vector semantics;
five unchanged source test files are also copied into the ordinary PPC suite.
`lmw` snapshots its effective address, sign-extends its displacement and
zero-extends each big-endian word. Local tests cover a negative displacement.

Re-review of Edge's `ppc_emit_fpu.cc` history found the follow-up
[`a5532a0d193130af1b5dd821c8c83daec27f6d11`](https://github.com/has207/xenia-edge/commit/a5532a0d193130af1b5dd821c8c83daec27f6d11)
(2026-10-05), "Clear only the exception bits in mcrfs". Class A/B: the original
`f4af1e2a7` implementation cleared control/status bits, losing XeFu's rounding
mode when reading field 7. Adapted the exception-only mask to static generation;
RN is read from the host-backed FPSCR rather than its RN-free `guest_bits`.
Local assembly tests check CR transfer, exception clearing, preserved rounding
and result flags. IBM's [mcrfs description](https://www.ibm.com/docs/en/aix/7.2.0?topic=set-mcrfs-move-condition-register-from-fpscr-instruction)
corroborates selective clearing. No XeFu execution result is claimed.

The complete ordinary instruction run exposed a second integration regression:
the existing `mtmsrd` lowering treated r13 as lock entry and every other GPR
as lock exit. Mapping `mtmsr` to it made standalone writes decrement an unheld
lock, contaminating subsequent `mfmsr` cases; Debug asserts on this path.
The modeled MSR mask and host critical-region mechanism remain, but locking
now follows EE transitions, allowing nested disable/restore pairs and repeated
enable writes. Generated test scopes release interrupt locks on case exit,
including exception paths; this is test isolation, not a title runtime reset.
Local assembly checks exercise both instructions and the enabled/disabled
`mfmsr` values. This does not implement a complete guest interrupt controller.

The reviewed Release corpus run removes all 3,333 #149 mismatches; the eight
previously classified hand-written corpus errors remain expected. Tests still
use the pinned corpus and skip list, not the newer upstream corpus. Arithmetic
exception causes and derived FPSCR summary bits remain a separate incomplete
runtime contract: `mcrfs` is validated for guest-written fields, not full
floating-point exception production. Final configuration/title evidence belongs
in [release evidence](release-evidence.md); no compatibility issue is closed
from the corpus result alone.

## RG-GDK-053: arithmetic FPSCR follow-up (2026-10-08)

Implemented sticky FPSCR updates for scalar arithmetic families in
`src/codegen/builders/floating_point.cpp`. Host fenv supplies inexact,
overflow, underflow and divide-by-zero flags; explicit operand classification
records PowerPC invalid subcauses. `FX`, `VX` and enabled `FEX` are derived as
arithmetic executes, while `mtfsf` continues to preserve guest-written status
bits for `mcrfs` compatibility. The tracked path also reports record-form CR1.

The same change now tracks compare invalid causes: `fcmpu` sets VXSNAN for
signaling NaNs; `fcmpo` sets VXVC for quiet NaNs and for signaling NaNs while
VE is disabled. Compare operations now set FPCC for less/greater/equal/unordered
and preserve C, FR and FI. Arithmetic result tracking updates FPRF using the
delivered result and target precision, including widened single-precision
subnormals. For an enabled invalid operation, the previous FPRF value is
preserved. The PowerPC User Instruction Set Architecture v2.02 specifies these
field encodings and the exception behavior ([FPSCR fields and result classes]).

`fctiw/fctiwz/fctid/fctidz` now track XX for inexact results, VXCVI for quiet
NaN/out-of-range conversions and VXSNAN for signaling NaNs. Record forms copy
the accumulated FPSCR state into CR1. This follows the IBM conversion status
descriptions, including the defined saturation results; FPRF is undefined for
these convert-to-integer instructions. Conversion FR/FI is tracked from the
rounded integer result, including directed rounding and inexact cases. The
`fcfid` family also records inexact status and FR/FI when an integer cannot be
represented exactly as a double. See [fctiw] and [fctidz].

[fctiw]: https://www.ibm.com/docs/en/aix/7.1.0?topic=set-fctiw-fcir-floating-convert-integer-word-instruction
[fctidz]: https://www.ibm.com/docs/en/aix/7.2?topic=is-fctidz-floating-convert-integer-double-word-round-toward-zero-instruction

`fres/frsqrte` now record zero-divide, signaling-NaN, negative-square-root
and reciprocal-overflow causes, and their record forms report accumulated
CR1. IBM's [fres] and [frsqrte] descriptions document these exception classes.
FR/FI is architecturally undefined for these estimate instructions and is
left unchanged.

[fres]: https://www.ibm.com/docs/ssw_aix_71/assembler/idalangref_fres_instrs.html
[frsqrte]: https://www.ibm.com/docs/en/aix/7.2.0?topic=set-frsqrte-floating-reciprocal-square-root-estimate-instruction

GDK Debug and Release builds of `unit_tests` and `ppc_tests` pass. The full
ordinary PPC CTest selection passes all 1,595 cases in both configurations.
The full unit CTest selection passes 542 cases with 14 documented private
asset/fixture skips in each configuration. The focused FPSCR
arithmetic/compare/conversion, existing `mcrfs` and unit tests pass under CTest
(28 cases per configuration). The tests
cover arithmetic/conversion exceptions, estimate zero-divide/overflow/invalid
cases, unordered NaN comparisons, sticky/summary state, enabled FEX/CR1 and
selective clearing. This is not full FPSCR coverage: estimate result-class
tracking and title/gameplay gates remain open. No title
compatibility claim follows from these synthetic tests.

The FPSCR result-class follow-up adds the defined FPRF classes for tracked
arithmetic results and FPCC relations for compares, with single-precision
subnormal classification based on target precision. The FR/FI follow-up
tracks inexact rounding direction for scalar arithmetic, fused arithmetic,
square root, single-precision rounding, integer conversion and integer-to-
double conversion. Directed-rounding and tie cases are covered. The focused
selection now passes 28 CTest cases in both GDK Debug and Release.

The estimate follow-up (2026-10-09) sets FPRF for `fres` (single-precision
class) and `frsqrte` (double-precision class), records `fres` underflow (UX)
when |x| > 2^126 makes the single reciprocal tiny, and leaves FPRF unchanged
when an enabled invalid or zero-divide exception suppresses the result, per
the [FPSCR fields and result classes] in ISA v2.02. Four new
`instr_fpscr_arithmetic.s` cases cover a normal result, a tiny single result,
a zero `frsqrte` operand and an enabled invalid `frsqrte`; three existing
estimate cases now expect the result class. GDK Debug and Release: the focused
FPSCR selection passes 29/29 CTest cases and the ordinary PPC executable
passes 1,599 cases / 6,559 assertions. Whether Xenon's `fres` signals UX for
an exact tiny result is not verified on hardware; the ISA's tininess rule is
used. Vector estimates do not touch FPSCR.

[FPSCR fields and result classes]: https://powerpc.dev/general/PPC_Vers202_Book1_public.pdf

The refreshed generated GDK Debug corpus passes all 566 active CTest groups
with the existing `instr_mtmsrd` timeout excluded. It exposed and verified
fused subtract sign handling for VXISI, NaN product classification and exact
preservation of guest-written FEX on `mtfsf`. The timeout remains unresolved;
this corpus result does not close #149 or establish title compatibility.

The Release SDK install regenerated and built Quantum of Solace (base and
TU2), Blood Stone and 007 Legends in fresh ignored title directories. Runtime
validation was held because the local `$SystemUpdate` bundle was unavailable,
which would leave the title without the Guide exit flow. Legends codegen also
reports one 1,437,835-byte function over the 1 MiB limit. Executable hashes
and these limits are recorded in [release evidence](release-evidence.md).
## Read-only Edge GPU review, 2026-10-07

Reviewed `furqanagwan/xenia-edge:edge` at
`669b4266f5682e5169d42552fdf64f02880652a4`; GitHub's comparison with
`has207/xenia-edge:edge` is identical (zero ahead/behind). The pinned
[GPU comparison](edge-gpu-review-20261007.md) distinguishes existing adaptations,
the optional DXIL source pin, missing `getBCF` and primitive invalidation fixes,
and coupled resolve/memexport changes requiring a separate design. Relevant
source diffs/history were inspected; no GPU code was ported in the GDK-policy
maintenance change. New candidates remain unvalidated and have no title
compatibility claim. The review does not import CPU JIT/XeFu execution,
Vulkan/Metal backends or a dependency on the reference emulator.

## getBCF translation, 2026-10-08

Source: `has207/xenia-edge:edge`, commit
[`669b4266f5682e5169d42552fdf64f02880652a4`](https://github.com/has207/xenia-edge/commit/669b4266f5682e5169d42552fdf64f02880652a4),
2026-10-07, `[GPU] Implement getBCF by sampling with forced border colors`.
The commit message describes sampling with transparent-black and opaque-white
border samplers and returning the largest component difference. It names filter,
LOD, mip, volume, stacked-texture and resolution-scale behavior, with cube maps
returning zero. The source changes span the shader model, DXBC/SPIR-V
translators, texture utilities and D3D12/Vulkan/Metal sampler caches. This port
adapts the DXBC and SPIR-V paths and D3D12 metadata only; the other backends are
outside the SDK target. No associated PR or source regression was identified in
the reviewed commit history.

Class A, GPU correctness. The local adaptation adds paired forced-border
samplers, preserves fully signed SRV selection, and adds synthetic point/linear,
signed, mip, volume, stacked and cube readbacks. Debug and Release fixtures pass
on Intel Graphics (driver 32.0.101.6129) and NVIDIA RTX 5080 Laptop GPU (driver
32.0.16.1742) through RTV and ROV. Strict DXIL fixture runs also pass in both
configurations on both adapters; each adapter/configuration/path runs the 290
assertions across the two fixtures. The reproducible shader-bytecode check,
roadmap validator, documentation checker, clang-format check and `git diff --check`
also pass. No AMD GPU is present for testing; CPU vendor does not
establish GPU-driver parity. The three representative 007 title runs remain
untested, so this is not a title compatibility claim or issue-closure evidence.
A separate tessellated-quad fixture still asserts `register_count() >= 2`; its
baseline and relationship to this work are unknown and it remains to be
triaged.

## Primitive conversion cache invalidation, 2026-10-07

Source: `has207/xenia-edge:edge`, commit
[`1b0e9d00ea33e6148c3429234e44e8c364edbd7d`](https://github.com/has207/xenia-edge/commit/1b0e9d00ea33e6148c3429234e44e8c364edbd7d),
2026-10-05, "Drop cached primitive conversions a guest write overlaps".
Reviewed at reference head `669b4266f5682e5169d42552fdf64f02880652a4` on October 7.
GitHub reports no associated PR or commit comments; it is a direct branch commit,
not an assumed merged Canary PR. No later changes to that source file appear
between the source and reviewed head. No affected game is named; external
regressions beyond that reviewed history remain unknown.

Class A, GPU correctness: a cached index range that a guest write contains or
ends at exactly its end was incorrectly kept. Coarse bucket writes also kept
entries, and writes during conversion did not inhibit insertion. Preserve the
SDK's `cache_mutex_` and physical callback dispatch rather than importing Edge's
global critical region or CPU/JIT machinery. Compare half-open overlap with
the write's start, mark an overlapping conversion invalid under that mutex,
and suppress insertion when its transaction finishes. This does not promise
an atomic snapshot of an in-flight draw; it prevents reusing a potentially
stale conversion on following draws. No new option or global title hack.

Baseline: the new CPU adapter reproduces four failures out of six initial
cases against the original production source; adjacent-range and unrelated
write controls pass. The adapted source plus a seventh same-bucket-entry
control passes Debug/Release. Production converter/shared-memory sources are
compiled into a dedicated software test target; the GPU plugin stays runtime
loaded for real consumers. Tests cover 16/32-bit indices, crossing cache bucket
boundaries, all exact overlap shapes, adjacency/empty ranges, coarse writes,
conversion-time writes and actual physical-memory watch dispatch plus refreshed
index contents. GPU/title results and missing gates are recorded in
[release evidence](release-evidence.md). Rollback is this isolated port; do not
fold shader or resolve redesigns into it.

A synthetic D3D12 triangle-fan readback also reproduces stale white geometry
after guest indices are changed to degenerate triangles, on both RTV and ROV
with the original source. The adapted source returns the red background as
expected, in Debug/Release. This exercises the real plugin, converted index
buffer and guest write watches rather than relying only on CPU allocation counts.
The affected RTV/ROV readback also passes strict DXIL in the opt-in GDK Release
configuration; the default shader path remains DXBC.

## furqanagwan/xenia-edge fork sync of 2026-10-09 (reviewed 2026-10-09)

Compared `furqanagwan/xenia-edge:edge` at `417b37dfeb` with the 2026-10-07
review point `669b4266f5`: 34 commits, 29 excluding merges. Classes follow the
A–H scheme above.

**Ported in this change**

| Edge commit | Change | Class | Adaptation and test |
| --- | --- | --- | --- |
| `19c23eb112` (Tim, 2026-10-07) | `upper_ascii` lowered instead of raising | A | Same one-line fix in `src/core/utf8.cpp`. It also repairs the Windows reserved-name check during disc extraction, which compared upper-cased names. `unit_tests [utf8]` |
| `3e9d62ffc4` (knuckleslee, 2026-09-29) | Handle `PM4_WAIT_IB_PFD_COMPLETE` | A | Indirect buffers run synchronously, so the packet is skipped. ReXGlue already advanced past unknown packets but asserted in Debug and logged each time |
| `0b041cd2a3` (The-Little-Wol, 2026-10-04) | `RB_HSIO_INTERFACE_ALIGNER_VALUE` register | A | Register added; reads return `0x00BBBBBB`; writes to it and to `RB_BC_CONTROL` are accepted silently |
| `0b0d57a1bc` (knuckleslee, 2026-10-03) | XAM message `0x2C009` (Kinect identity callback) | B | Accepted with `X_E_SUCCESS`; the callback is not invoked. Fixes 5345084D stopping Kinect at startup upstream; no local Kinect title |

**Relevant, not yet ported (GPU, need a design and title checks)**

| Edge commit | Change | Notes |
| --- | --- | --- |
| `ac37478be6`, `7347cd12ef` (knuckleslee, 2026-10-03 / 2026-09-30) | `k_DXT3A_AS_1_1_1_1` component bit order; resolve copy command 2 into that format | **Ported 2026-10-11 (#236), class A.** Texture loaders read red from bit 0; new `resolve_dxt3aas1111` shader rewritten in ReXGlue's older shader dialect (`...AddressInts`, uint vector buffer); unscaled only, so a scaled resolve skips the mask copy but still clears. GPU fixture `Copy command 2 packs one bit per component...` (red in bit 0, alpha in bit 3; untouched at 3x2). See [research](../research/gpu/dxt3a-as-1111.md) |
| `6579cc2a4c` (Tim, 2026-10-01) | Host viewport may start at a negative coordinate | **Ported 2026-10-11 (#236), class A.** `ViewportInfo::xy_offset` is signed and the D3D12 viewport may start at -max. Test `A tile drawn at a negative window offset keeps the full viewport` (-608, Blue Dragon's case), which fails on the old code. 007 title runs batched with the rest of #236 |
| `e1f4a0d009` (goldislead, 2026-10-01, xenia-canary) | Tiling window offsets moved into EDRAM bases | **Ported 2026-10-11 (#236), class A, from the Canary commit.** Adapted to ReXGlue's older render target cache (no Canary `scale_native` key bit) and split files; the DXIL path's SPIR-V constants, FSI EDRAM bases and viewport cache key carry it too. Setting `window_offset_in_edram` (on). Unit tests `[graphics][draw]` window offset cases. See [research](../research/gpu/window-offset-edram.md) |
| `cc4981a465` (goldislead, 2026-08-31, xenia-canary) | Rasterize at Xenos MSAA sample positions | **Ported 2026-10-11 (#236), class A, from the Canary commit.** ROV (DXBC) and FSI (SPIR-V/DXIL) take coverage from the host samples nearest each Xenos position and evaluate depth at the Xenos positions; host render targets use programmable sample positions only with `xenos_sample_positions` (off by default, as upstream), with MSAA depth decompressed before shader reads (`TransitionRenderTarget`). Default NVIDIA/AMD path (RTV, setting off) unchanged. Unit tests `[msaa]`; GPU fixtures 59/59 under ROV and under `xenos_sample_positions=true` (RTX 5080, positions tier 2). See [research](../research/gpu/xenos-sample-positions.md) |

**Deferred or not applicable**

- `22a39ec9d6` `XamUpdateGet*SystemVersion` returns Edge's build 1888, which
  contradicts the kernel version ReXGlue reports (build 0xFFFF); deferred.
- `5629c82ab1` XHttp stubs, `82d0cd1f44` XAM UI stubs (multiplayer and Gold
  upgrade, NUI dirty disc), `7ba916c010` XEX header logging,
  `f41ee57bc3` user-properties mutex (ReXGlue has no equivalent property
  store), `1406d87095` null audio driver: no local title needs them; revisit
  on demand.
- `828014b908` storage-devices-changed notification: ReXGlue never sends it
  on registration.
- `669c9709d2` rename by guest-path name: POSIX only; Windows `filename()`
  already splits on `\`.
- `b083312b8b` `kInvalidVertex` draws, `6260a87b85` wide 1D fetch: already
  recorded above.
- Posix, Linux, Vulkan, `xb` build and OG Xbox commits: out of scope
  (`f65fda3320`, `6435e79904`, `b9e00e6cce`, `8249bbe381`, `bdcc36f3b3`,
  `2416330549`, `2e6898afc1`, `44f5b4a860`, `c8f214e2a9`, `4a3ecf02d5`).

## RG-GDK-032: Edge shader parsing fixes on both paths (2026-10-09)

has207/xenia-edge `92ada8ebc0` ("[GPU] Fix scalar ALU swizzles with
three-source vector ops", 2026-08-13). Class A (correctness, all titles,
DXBC and DXIL). A two-component single-operand scalar op (`adds`, `maxs`,
...) reads source 3's Z rather than X as its right-hand operand when the paired
vector op uses three sources. Adaptation: `AluVectorOpcodeInfo::ScalarSecondSourceComponent`
shared by the parser and the shader interpreter; no comments, the rule is in
`research/gpu/shader-operand-components.md`. Edge `fbdb1f2817` (1D fetches
with XY coordinates, 545407D4) was already in through the getBCF port
(`80bb713`) on both translators, so nothing further was taken. Tests:
`shader_parse_tests` (new, default build; fails on the old parser), the
Release suite and `gpu.dxil_parity`. Known regressions: none at the pin; the
007 titles are checked in the next title batch.

## RG-GDK-032: hybrid occlusion counting on the DXIL path (2026-10-09)

No new upstream code. The SPIR-V translator's `zpd_total` modification and
`FBO_AddMSAASamplesToZPDTotal` came with the stage 2 port (has207/xenia-edge
`0788c561e3`); this wires them up in ReXGlue's D3D12 draw path, as Edge's
`d3d12_command_processor.cc` sets `set_zpd_total` for hybrid draws. Class C
(DXIL path only; DXBC unchanged). Not taken: Edge's later ZPD rewrite as a
running sample counter (`3d233a5b2e`), which ReXGlue's segment model doesn't
use. Adaptation: draws without a guest pixel shader stay on DXBC's depth-only
counting shader; a pending counting pipeline draws with the DXIL pipeline
without counting. Tests: `[hybrid]` ZPD fixtures and `gpu.dxil_parity` in
strict mode, with a mutation check (counter index off: 9 assertions fail).
Known regressions: none.

## RG-GDK-032: wide 1D textures on both paths (2026-10-09)

has207/xenia-edge `947075f880` ("[GPU] Implement wide 1D texture support",
2026-08-04), `7cd47947b0` ("[GPU] Select promoted tfetch1D layouts at
runtime", 2026-08-18) and `6260a87b85` ("[GPU] Fix fetching from wide 1D
textures at large coordinates", 2026-10-03). Titles: 5345084D, 555308CE
(Edge's reports; not tested here). Class B (compatibility; DXBC and DXIL).
1D textures wider than 8,192 texels become 2D textures of 8,192-texel rows
(at most 128) instead of being dropped, and 1D fetches map their coordinate
to a row at runtime. The SPIR-V translator already had the first two commits
from the stage 2 pin; this adds the texture cache, the guest layout, the
DXBC translator and the third commit on both translators. Adaptation: the
DXBC remap is a separate `EmitWide1DTextureCoordinates`; promoted fetches
keep ReXGlue's static 2D dimension, with the 1D/2D size picked at runtime;
point-sample snapping follows the original fetch dimension, so promoted
fetches no longer snap (as on Edge). Not taken: Edge's two-component 1D
gradients and ignoring register gradients for 1D. Tests:
`tests/gpu/wide_1d_texture_fixture_test.cpp` (14 wide samples fail without
the texture cache change), `texture_layout_test.cpp`, the Release suite and
`gpu.dxil_parity`. Known regressions: none at the pin; the 007 titles are
checked in the next title batch. Notes: `research/gpu/wide-1d-textures.md`.

## RG-GDK-032: frame end doesn't wait for queued pipelines (2026-10-09)

has207/xenia-edge `4366b05dae` ("[D3D12] async shader compilation actually
fully async", 2026-01-06). Class A (correctness of async compilation; DXBC
and DXIL). With `async_shader_compilation`, `PipelineCache::EndSubmission`
no longer creates and awaits every queued pipeline; skipped draws never
record one that isn't ready. Adaptation: the wait stays when async
compilation is off; Edge's `use_try_claim` hunk (placeholder pipelines) not
taken. Tests: Release suite and `gpu.dxil_parity` (async draw fixtures); a
paired QoS DXIL run, cold cache, same save: level-load frame 25.5 s → 7.5 s,
5 s windows with a frame over 300 ms after the load 16 → 0. Known regressions: none.
Notes: `research/gpu/async-pipeline-creation.md`.

## RG-GDK-032: hybrid occlusion counting without a pixel shader on DXIL (2026-10-09)

No new upstream code: the SPIR-V translator's depth-only `zpd_total`
fragment shader came with the stage 2 port (has207/xenia-edge `0788c561e3`).
Class C (DXIL path only). Hybrid draws without a guest pixel shader no
longer fall back to DXBC: `InitializeDxilHelperPixelShaders` converts the
counting depth-only shader (and its float24 variants) and
`GetDxilHelperPixelShader` picks it for a `zpd_total` description. Tests:
the `[hybrid]` ZPD fixtures and `gpu.dxil_parity`, strict; mutation check
(the plain depth-only shader in its place fails "RTV full counters count a
draw without a pixel shader"). Not run in a title: QoS doesn't enable
`occlusion_query_full_counters`. Known regressions: none.

## XamContentResolve package paths (2026-10-11)

xenia-canary `canary_experimental` at `c9c8e483` (`xeXamContentResolve`
in `src/xenia/kernel/xam/xam_content.cc`; the audit clone is shallow, so the
commit that added it is not pinned). Class A (an export that failed outright
now builds the console's content package path). Adaptation: the path builder
is `ResolveContentPackageDevicePath` in the content manager, and the export
calls it. Canary's `XamContentResolveInternal` is not taken, since no
representative title imports it. Tests: `kernel_tests` `[content]` path cases.
Title effect: Legends' only call no longer warns. The package open that
follows still fails, because ReXGlue stores packages as folders. Known
regressions: none. Notes: `research/kernel/title-startup-queries.md`.
