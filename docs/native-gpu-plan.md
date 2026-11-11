# Native GPU plan

Status: plan, 2026-11-10. Decided in
[ADR-018](adr/ADR-018-re360-one-repository-two-gpu-plugins.md). It started as
research written when ReCPU (the ReXGlue SDK) and ReGPU were two repositories;
this version is updated for Re360, where they are one.

## Summary

Any game recompiled with Re360 gets native CPU code today. The goal is the same
for the GPU: shaders translated to DXIL ahead of time and a lean host renderer,
with no per-game work. Removing the Xbox 360 command processor entirely stays
per-game work, because the PM4 ring buffer is the only interface every game is
guaranteed to use.

Three tracks, in this order:

1. **Shaders ahead of time in the `xenos` plugin.** Find a game's shaders at
   build time, translate them offline to DXIL, and ship them with the
   executable. Runtime translation becomes the fallback.
2. **ReGPU as the lean renderer.** It keeps render targets as host textures,
   needs no in-shader EDRAM emulation and emits readable HLSL. The plugin now
   exists (`gpu_plugin=regpu`); it has to reach gameplay on real titles.
3. **Per-game native renderers, opt-in.** For games worth it, hook the game's
   own Direct3D calls the way UnleashedRecomp and Skate 3 Recomp do. This needs
   an XDK signature database first; `SigScanner::hleSignatures()` is empty.

## Where things stand

| Part | What it does now | Gap for this goal |
| --- | --- | --- |
| Codegen (`src/codegen`) | PowerPC to C++ ahead of time; no JIT (ADR-004) | Doesn't touch shaders or name XDK functions |
| `rexgpu-xenos` (`src/graphics`) | Xenia-derived Xenos emulation: PM4, EDRAM render-target cache, texture untiling, runtime shader translation | Translates shaders while the game runs; shaders carry EDRAM/ROV logic and depend on render-state modifications |
| DXBC and DXIL shader paths | DXBC (SM 5.1) ships. Opt-in DXIL via Edge's SPIR-V translator and Mesa `spirv_to_dxil` (`gpu_shader_path=dxil`) | DXIL compiles on the player's PC; a cold start on Quantum of Solace stalled for up to 21.5 s |
| Shipped shader cache (`docs/shader-cache.md`) | Ships recorded shader and pipeline records | Needs someone to play the game first, and still translates on the player's PC |
| GPU plugin boundary (`include/rex/system/gpu_plugin.h`) | `IGraphicsSystem`, loaded from `rexgpu-<name>.dll`, ABI 2 | Two plugins exist: `xenos` and `regpu` |
| `rexgpu-regpu` (`src/regpu`) | ReGPU behind the plugin interface: host surfaces for EDRAM, microcode to HLSL, FSR/DLSS, a memory scan for shaders at load | Draws, resolves and presents in fixtures (`gpu.regpu_plugin_smoke`, `gpu.regpu_present`); not yet run on a title here. Runtime FXC, no tessellation, memexport writes dropped, fixed 1280x720, no Guide overlay |

## What "native GPU" can mean

Levels 1 and 2 work for any game; level 3 is a port per game. They stack: each
level keeps the one below as its fallback and reference image.

| Level | What runs on the GPU | What is still emulated | Generic? | Prior art |
| --- | --- | --- | --- | --- |
| 1. AOT shaders | Every shader translated from Xenos microcode to DXIL at build time, keyed by microcode hash | Command processor, EDRAM, resolves, texture untiling | Yes, if the shaders can be found at build time | UnleashedRecomp's XenosRecomp step; Xerenge's build-time shader recipe |
| 2. Lean host renderer | Level 1, plus render targets as real host textures, resolves as copies, no rasterizer-ordered views | A thin PM4 reader and register file | Yes. The PM4 stream is common to every game | ReGPU (Forza Motorsport 2) |
| 3. D3D-level native renderer | The game's own `D3DDevice_*` calls go to host code; no ring buffer at all | Nothing on the GPU side | No. Each game (or engine) needs its own renderer | UnleashedRecomp; Skate 3 Recomp 2.0; Xerenge's `plume` plugin |

Why level 3 can't be generic: the XDK's Direct3D is statically linked into each
game, differs by XDK version, and a game may write commands into the ring
without calling an API function per draw. A hook at
`D3DDevice_DrawIndexedVertices` sees only what the game routes through that
function. The PM4 stream sees everything.

Finding shaders at build time is the hard part of level 1. Three sources, used
together:

- **The executable and game files.** Shader objects embed a ready-made
  `IM_LOAD_IMMEDIATE` packet (header `0xC0nn2B00`) before the microcode. ReGPU
  already scans guest memory for this pattern at load
  (`src/regpu/gpu/shader_scan.cpp`); the same scan can run over `default.xex`
  and the game's files at codegen time.
- **Recorded runs.** The shader storage already records every shader a run
  translates. These records become a per-game recipe of hashes, with no game
  data in it.
- **Runtime fallback.** A shader that wasn't found at build time is translated
  on the player's PC, as today, and recorded so the next build includes it.

## Phases

Each phase is useful on its own and ends in a gate recorded in
[release evidence](release-evidence.md). Validation follows AGENTS.md: the three
007 titles, fullscreen, one candidate run per check, NVIDIA required, AMD and
Intel where available (ADR-007).

### Phase 0: measure

- Spike on Quantum of Solace: scan `default.xex` and the game files for
  `IM_LOAD_IMMEDIATE` shader packets, then compare the hashes with the shaders
  its recorded cache holds. The overlap is the build-time coverage number every
  later phase depends on.
- In the same spike, trace one frame: how many draws reach PM4 through a
  callable `D3DDevice_*` function, and how many don't. This settles how far
  level 3 could ever be shared between games.
- Close ADR-008's licence check for Mesa, Slang and `dxil.dll`.
- **Gate:** both numbers written up in `research/`.

### Phase 1: shaders ahead of time in `rexgpu-xenos` (level 1)

- Add a `rexglue shaders` step after `rexglue codegen`. It scans the executable
  and game files, merges recorded shader and pipeline records, and writes a
  per-game list of hashes (XXH3, the key the shader storage already uses).
- Translate each shader offline with the existing SPIR-V translator, for every
  modification the recorded pipeline records name. Convert with Mesa
  `spirv_to_dxil` and sign with the pinned `dxil.dll`.
- Ship the result as a shader pack beside the executable. The plugin looks up
  (hash, modification) before translating; a miss translates as today and is
  recorded for the next build.
- The pack is built from the player's own game, like generated C++, and never
  committed.
- **Gate:** all three 007 titles, cold start with no cache, DXIL path: no failed
  pipelines, worst frame at or below the warm-cache DXBC run, screenshots
  matching. `gpu.dxil_parity` still passes.

### Phase 2: ReGPU on real titles (level 2)

Done: ReGPU is in the tree, `rexgpu-regpu` implements `IGraphicsSystem` on
`regpu::driver`, maps the `0x7FC80000` register range and fills `regpu::Host`
from the runtime's memory, threads and guest calls. Fixtures draw from Xenos
microcode, resolve to guest memory and present into a host-owned window.

Still to do:

- Run the three 007 titles on `gpu_plugin=regpu` and fix what breaks, in Xenos
  terms ([ReGPU debugging](regpu-debugging.md)).
- Close ReGPU's generic gaps: a guest resolution setting instead of the fixed
  1280x720, memexport, tessellation.
- Present through the runtime's presenter so the Guide and overlays work, and
  read the runtime's cvars.
- Move ReGPU onto the runtime's logging, device and image code, one piece at a
  time with a picture to compare.
- Give ReGPU the Phase 1 treatment: its HLSL goes through DXC at build time into
  its own shader pack, replacing runtime FXC.
- **Gate:** all three 007 titles reach gameplay on `regpu`; paired screenshots
  and frame times against `xenos`. Then pick one translator for ahead-of-time
  shaders.

### Phase 3: name the game's graphics functions

- Fill `SigScanner::hleSignatures()` (`src/codegen/sig_scanner.cpp`) with a
  signature database for the XDK's D3D, XGraphics, XAudio and CRT libraries, per
  XDK version, matched at codegen (as RG-FIX-006 already does for `setjmp` and
  `longjmp`).
- Output: readable names in generated code and a per-game symbol map.
- **Gate:** the D3D draw, resolve and swap functions named in all three 007
  titles (three XDK versions), with no false matches against a hand-checked list.

### Phase 4: first per-game native renderer (level 3)

- In a title repository, not here (ADR-005, ADR-009). Start on the smallest
  surface: Quantum of Solace's menus and HUD, then one full scene.
- It hooks the named D3D functions with `REX_HOOK`, draws with the Phase 1
  shader pack, and falls back to `xenos` or `regpu` per run with a cvar.
- Build the comparison harness first: the same frame captured both ways and
  diffed with tolerances.
- **Gate:** the scene matches the emulated reference within tolerance; no
  regressions on the other two titles.

## Risks

| Risk | Why it matters | Mitigation |
| --- | --- | --- |
| Build-time shader coverage is low | Some games build or patch shaders at runtime (vertex-fetch fields patched into shader copies) | Recorded runs fill the gap; runtime translation stays as the fallback |
| Too many shader variants | `rexgpu-xenos` translates each shader per render-state modification | Build only the (hash, modification) pairs recorded runs name; ReGPU's simpler shaders need fewer variants |
| Two GPU stacks to maintain | Two command processors and two translators (SPIR-V and HLSL) | Keep `xenos` as the reference; pick one translator after the Phase 2 numbers |
| ReGPU is proven on one game | Forza Motorsport 2, under XenonRecomp, at 1280x720 | Phase 2 gate on all three 007 titles before any default changes |
| Game-derived output in the repository | DXIL and HLSL translated from a game are the game's shaders | Shader packs are built locally from the player's copy and git-ignored |
| Validation hardware | One NVIDIA laptop GPU is recorded; AMD is blocked (ADR-007) | Record vendor gaps as blocked, never as passing |

## Sources

- [Native renderers](native-renderers.md), [DXIL shader toolchain](shader-dxil.md),
  [shader cache](shader-cache.md), [shader replacements](shader-replacements.md),
  ADR-004, ADR-008.
- [XenosRecomp](https://github.com/hedge-dev/XenosRecomp): Xenos shaders to HLSL.
- [UnleashedRecomp build docs](https://mintlify.wiki/hedge-dev/UnleashedRecomp/building/recompilation):
  shaders translated once, no GPU emulation at draw time.
- [Skate 3 Recomp](https://github.com/mchughalex/skate3recomp): native D3D12 and
  Vulkan renderer since v2.0.0.
- [Xerenge](https://raw.githubusercontent.com/shipa-2/Xerenge/HEAD/README.md):
  `plume` GPU plugin on ReXGlue, shaders from scanning the executable plus a
  runtime recipe.
- [Cxbx-Reloaded: OOVPA signatures](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Maintaining-OOVPAs-for-HLE-function-detection):
  per-XDK-version function detection on the original Xbox.
