# Native renderers: foundations (research)

Status: research, 2026-09-29. The architecture decision it asks for is
[ADR-018](adr/ADR-018-re360-one-repository-two-gpu-plugins.md) (2026-11-10), and
the phased route is the [native GPU plan](native-gpu-plan.md). Earlier tracking:
[#304](https://github.com/furqanagwan/rexglue-sdk/issues/304). The DirectX 12
Ultimate capability layer native renderers build on is decided in
[ADR-017](adr/ADR-017-directx-12-ultimate-tier.md).

## The goal

Today a recompiled title draws the way the Xbox 360 did. Its statically linked
XDK Direct3D library writes PM4 command packets into a ring buffer, and
ReXGlue's GPU plugin (`rexgpu-xenos`) emulates the Xenos GPU: PM4 parsing,
EDRAM render targets, resolves, texture untiling, and microcode shaders
translated to DXBC at runtime.

A native renderer would draw each title with host D3D12 directly: the game's
own renderer calls reach host code, and there is no Xenos in between. That
removes EDRAM emulation and ring-buffer synchronization, and frees resolution,
aspect ratio, frame rate and effects. The best-known example is UnleashedRecomp
(hedge-dev), which recompiles Sonic Unleashed with XenonRecomp (ReXGlue's
codegen ancestry) and draws it natively: it replaces the game's D3D calls and
translates its shaders ahead of time with XenosRecomp.

This is title-specific work (ADR-005). The SDK's job is the shared foundation
that makes each title's renderer small, testable and safe to develop next to
the emulated path.

## What exists already

| Need | In ReXGlue today |
| --- | --- |
| Replace a guest function with host code | `REX_HOOK` / `REX_HOOK_RAW` (`include/rex/hook.h`), codegen function overrides and mid-asm hooks (`docs/code-patches.md` covers byte patches) |
| Call back into guest code from host code | `REX_IMPORT`, `CallFrame`, `StackFrame` |
| Swap the GPU implementation | The `gpu_plugin` boundary (`include/rex/system/gpu_plugin.h`); an empty value loads `xenos` when `rexgpu-xenos.dll` is next to the executable, and `none` disables GPU emulation |
| Xenos formats, tiling, fetch constants | `src/graphics/pipeline/texture` (untiling, conversion), `include/rex/graphics/xenos.h` |
| Xenos microcode to host shaders | DXBC translator, runtime and EDRAM-aware (`dxbc_translator*.cpp`); DXIL path planned (ADR-008, #53) |
| Reference images | Emulated path, PIX frame capture (`d3d12_capture_frame`), baseline screenshots (`scripts/capture_baseline.py`) |
| Frame pacing numbers | `frame_stats_interval` |

## What has to exist first

In dependency order. Each is useful on its own, even before any title renderer.

### 1. An architecture decision (ADR)

Settle before writing code:

- **Where the renderer hooks.** Either at the **XDK D3D API**
  (`D3DDevice_SetTexture`, `DrawIndexedVertices`, `Resolve`, `Swap`...), which is
  one boundary that is similar across titles built on the same XDK version,
  or at the **engine's own renderer** (IW for Quantum of Solace, Bizarre
  Creations' engine for Blood Stone, Eurocom's for 007 Legends), which is fewer,
  higher-level calls but is a different design per engine. UnleashedRecomp works at
  the D3D level.
- **Where the code lives.** The shared layer belongs in the SDK and each title's
  renderer in its title repository (ADR-005, ADR-009). Candidates: a title
  GPU plugin (`rexgpu-<title>`), or title hooks plus an SDK "host renderer"
  library.
- **How it coexists with emulation.** Mixing native and emulated draws in one
  frame means sharing EDRAM state, which is very hard. The practical model is
  one or the other per run, selected by a cvar, so the emulated path remains
  the fallback and the reference.
- **What the guest still expects.** The game's D3D waits on GPU progress
  (Quantum of Solace spins in D3D's ring-space wait, `sub_820E7098`), reads
  fences and vblank counters, and calls `VdSwap`. A native path has to satisfy
  those without a ring buffer.

### 2. Naming the guest's graphics functions

Recompiled functions are `sub_XXXXXXXX`. A renderer needs to know which address
is `D3DDevice_DrawIndexedVertices` in each title. The XDK D3D library is
statically linked, and its code differs by XDK version (Quantum of Solace 2008,
Blood Stone 2010, Legends 2012).

- **Tool:** a signature database for XDK libraries (D3D, XGraphics, XAudio,
  CRT), matched at codegen time, as RG-FIX-006 already does for CRT `setjmp` and
  `longjmp`. The matches name functions in the generated code and export a
  per-title symbol map.
- **Benefit on its own:** readable generated code, fault reports and profiles.
  The profiling for RG-GDK-037 needed manual address mapping
  (`PPCFuncMappings`) to find `sub_820E7098`.

### 3. Typed guest D3D structures

Hooks receive guest pointers to `D3DDevice`, texture, vertex and index buffer
headers, and shader objects. The SDK needs typed, endian-correct views of these
Xbox 360 D3D layouts, versioned by XDK where they differ, next to the existing
fetch-constant definitions. Without them each title renderer reverse-engineers
the same structures again.

### 4. Offline shader translation

A native renderer knows its shaders ahead of time: they are in the game files
or created at load. It needs:

- extraction of every microcode shader a title uses (at load, keyed by ucode hash);
- translation to a readable intermediate (HLSL), without EDRAM and ROV
  emulation, which the translator currently mixes into every pixel shader;
- compilation to DXIL at build time.

This overlaps with ADR-008 and #53: whichever IR the SDK settles on should
serve both the emulated path and offline translation. It is the largest single
foundation.

### 5. A resource library independent of the command processor

Texture untiling and format conversion, and vertex and index format decoding,
are currently driven by the PM4 command processor and its caches. A native
renderer needs the same conversions called directly ("upload this guest texture
header"), with guest-memory write tracking so it can invalidate cached data.
That means separating the conversion code from `TextureCache` and
`SharedMemory` into a library with its own tests.

### 6. Render-to-texture and resolves

Xbox 360 games draw into EDRAM and resolve into textures. Post effects, shadows
and reflections all go through this. A native renderer maps each resolve to a
host render target or copy. That needs a per-title inventory of render passes,
which the frame capture tools can produce, and a documented mapping in the
title repository.

### 7. A comparison harness

A native renderer is correct when it matches the emulated path. Needed:
capture the same frame both ways, at the same guest state, and diff the
images, with tolerances. The pieces exist (PIX capture fixture, baseline
screenshots, GPU fixtures); they need to be joined into one test that runs
per title and per scene.

## Suggested order

1. The ADR (hook level, code location, coexistence, guest expectations).
2. XDK signature naming (2), which pays off immediately for debugging.
3. Typed D3D structures (3) and the resource library split (5).
4. Offline shader extraction and translation (4), aligned with #53.
5. The comparison harness (7).
6. A first title prototype on the smallest surface, for example Quantum of
   Solace's 2D menus and HUD, then one full scene, with the emulated path as
   the fallback.

## Risks and costs

- **Per-title scale:** each renderer is a port, not a patch. UnleashedRecomp's
  renderer is a large, dedicated codebase for one game.
- **Engine knowledge:** resolve chains, predicated tiling and memexport
  (particles, skinning) have to be understood per title.
- **Two paths to maintain:** emulation stays for fallback, other titles and
  as the reference.
- **Legal boundary:** shaders and data extracted from a disc stay private, like
  generated code; only tools and hand-written code are shared.

## Why the emulated path still matters

The native renderer is a long-term goal. The emulated path is still what runs
all three titles today, and fixes there, such as the Blood Stone visual bug
under investigation, help every title immediately.
