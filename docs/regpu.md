# ReGPU: the `regpu` GPU plugin

ReGPU is Re360's second Xenos renderer. It reads the same PM4 command stream as
the default `xenos` plugin but keeps render targets as host textures, turns
resolves into copies, and translates shader microcode to readable HLSL. It can
upscale (FSR 1 built in; FSR 3 and DLSS as build options).

What is proven here, on a real Direct3D 12 device through the plugin interface:

- `gpu.regpu_plugin_smoke`: a PM4 command stream, guest threads, the register
  range, and draws shaded from Xenos microcode (translated to HLSL, compiled,
  drawn at 1x and 4x MSAA) resolved to guest memory with the exact colours.
- `gpu.regpu_present`: a 1280x720 frame swapped into a window the host owns,
  read back from the presented picture with the colour it drew. The same at
  1024x600 and 1152x640, the sizes some titles render at.

James Bond 007: Blood Stone runs on it in Re360 (2026-11-11, NVIDIA RTX 5080
Laptop GPU): title screen, menus, the first levels and the Xbox Guide over the
game, fullscreen at 3840x2160. Its first level rendered with green floors and a
ghosted picture until two EDRAM fixes (aliased depth fill, resolves through
another MSAA view; see the [Xenos notes](../research/gpu/regpu-xenos-notes.md)).
Quantum of Solace and 007 Legends have not been run on it yet; that is the rest
of the gate in the [native GPU plan](native-gpu-plan.md).

## Use it

A project made with `rexglue init` stages both plugins. Pick ReGPU per run:

```powershell
MyGame.exe --gpu_plugin=regpu
```

An existing project adds `regpu` to its `GPU_PLUGINS` list
(`rexglue_setup_target(MyGame GPU_PLUGINS xenos regpu)`) and rebuilds.

ReGPU writes its shader and pipeline caches under the title's cache folder, in
`regpu`. Its log is `regpu.log` in the working folder (`REGPU_LOG_DIR` moves it).
The first run of a game compiles its shaders and can take minutes; later runs
load them from the cache.

## How it is wired

| Piece | Where | What it does |
| --- | --- | --- |
| The renderer | `src/regpu/gpu`, `src/regpu/render` | Command processor, microcode to HLSL, EDRAM surfaces, resolves, textures, presentation |
| Its API | `include/regpu/regpu.h`, `include/regpu/driver.h` | `Config`, `Host` and the entry points the kernel's `Vd*` exports reach |
| The plugin | `src/regpu/plugin/graphics_system.cpp` | Implements `IGraphicsSystem` (`include/rex/system/interfaces/graphics.h`) on top of that API |

The plugin gives ReGPU what it asks of a host: guest memory, guest allocations,
threads that can run guest code, and the graphics interrupt through the
function dispatcher. It routes the guest's GPU register range (`0x7FC80000`) to
ReGPU's register file.

ReGPU renders on the runtime's Direct3D 12 device and queue and hands each
finished frame to the runtime's presenter (`Host::presentFrame` and
`regpu::CopyFrameTo`), as the `xenos` plugin does. So the window, fullscreen,
the Xbox Guide, notifications and the other overlays are the runtime's and work
the same on both plugins.

## Shaders at setup

Before the game starts, the runtime asks the GPU plugin to prepare its shaders
(`InitializeShaderStorage`). ReGPU compiles every shader and pipeline earlier
runs stored and reports progress (`GetShaderPreparation`), which the runtime
shows on the Guide's list page as **Preparing Shaders**, then starts the game.
After an update that changes the translator, or with a cleared cache, this is
where the rebuild happens instead of during play (Blood Stone: 465 shaders,
755 pipelines, 12 s). A first-ever run has nothing stored and shows no page.

Scanning the game's files for shaders ahead of the first run does not work
for every title: Blood Stone keeps its shaders compressed in
`cache/shaders-speed.pak`, and a scan of its 6.6 GB of files finds none. The kernel's video exports (`src/kernel/xboxkrnl/xboxkrnl_video.cpp`) are
shared by both plugins; ReGPU's own copies were removed in the merge.

## What it does not do yet

- **Runtime settings.** `resolution_scale` and the other GPU cvars belong to
  the `xenos` plugin. ReGPU renders at the game window's size when the game
  starts and takes its switches from `REGPU_*` environment variables.
- **Frame size.** The renderer starts at 1280x720 and follows the front buffer
  size the title's first swap reports, so a title that renders 1024x600 shows
  its first frame wrong and the rest right. Front buffers above 1280x720 are
  untested (see the [Xenos notes](../research/gpu/regpu-xenos-notes.md)).
- **No tessellation; memexport writes are dropped.**
- **Shaders compile on the player's PC** with FXC (`d3dcompiler_47.dll`) by
  default: at setup when earlier runs stored them, else during play. The
  opt-in DXIL shader pack below builds them ahead of time.
- **One renderer per process, and no teardown.** Its threads run until the
  process exits, which is how titles exit today.
- **Its own support code.** ReGPU still carries its own logger, PNG writer,
  live switches and D3D12 device setup instead of the runtime's. They are
  small, and replacing them is safe only once a game runs on it here.
- **The debug server's controller** (`regpuctl.py press`) is not merged into
  the runtime's input yet.

## DXIL shader pack (opt-in)

`REGPU_DXIL=1` makes every translated shader DXIL, compiled with DXC instead
of FXC. A pipeline takes DXBC or DXIL for all its stages, never a mix, so it is
one mode for the whole run. Each shader comes from, in order:

1. the shader pack: `regpu_shaders\<key>.dxil` beside the game executable
   (`REGPU_SHADER_PACK` moves it), built ahead of time;
2. the shader cache (`shadercache\<key>.dxil`);
3. DXC on the player's PC, for a shader the pack doesn't have.

The key is a hash of the translated HLSL, so a translator change misses the
pack and falls back to step 3 rather than loading stale code.
The game needs `dxcompiler.dll` and `dxil.dll` beside it (`dxil.dll` may
instead be in its `D3D12` folder); `dxil.dll` signs the output, and D3D12
rejects unsigned DXIL. Without `dxcompiler.dll` the run stays on FXC.

Configure the SDK with `-DREGPU_DXIL_PACK=ON`. It downloads DXC 1.8.2502.8
(pinned in `cmake/rexglue_dxc.cmake`, the version Microsoft's PC backward
compatibility ships) and installs `regpu_dis` with both DLLs in `bin`. A title
that stages the `regpu` plugin then gets the two DLLs beside its executable
and a `<title>_regpu_shaders` target that builds the pack from the shader cache
a run left (`%LOCALAPPDATA%\<title>\cache\regpu\shadercache`, or
`-DREGPU_SHADER_CACHE=<folder>`):

```powershell
cmake --build out/build/release --target bloodstone_regpu_shaders
```

Play with `REGPU_DXIL=1` after a pack build and the shaders the pack lacked
are recorded in the cache; the next pack build includes them.
`regpu_dis --pack <pack folder> <cache folder>` does the same by hand.

Blood Stone, 2026-11-11: 222 shaders pack in 18 s (5.6 MB; FXC's cache is
14 MB and takes 119 s optimised). With the pack, a 3-minute run in the Istanbul
cistern loaded every shader from it at setup and compiled 5 with DXC during
play, at 30 fps with no failed pipelines; the picture looks as with FXC. The
pack is built from the player's own game files and is never committed.

DXC compiles with strict IEEE arithmetic (`-Gis`). `regpu_shadercmp <cache
folder>` checks the two compilers agree: it runs every shader twice with
identical inputs, as FXC builds it at run time and as the pack holds it, and
compares the 32-bit float results.

- Pixel shaders are drawn over a 256x256 target with fixed pseudo-random
  constants, noise textures and varying interpolators.
- Vertex shaders run over 192 vertices whose fetches read random floats
  through the shader's own fetch layout. Their outputs (position after the
  viewport transform, the 16 interpolators, the point coordinate) are captured
  by stream output, without rasterising.

Blood Stone, 190 pixel and 136 vertex shaders:

| DXIL build | Pixel: identical / within 0.1% / differ | Vertex: identical / within 0.1% / differ |
| --- | --- | --- |
| DXC default (fused, reassociated arithmetic) | 61 / 95 / 34 | not run |
| DXC `-Gis` | 141 / 49 / 0 | 113 / 23 / 0 |
| FXC optimised, as a control (`--control`) | 141 / 49 / 0 | 88 / 48 / 0 |

(The default-arithmetic row was measured before guest memory held random
data, which moved 5 pixel shaders from "within" to "identical" in the other
rows.) The default build's differences came from multiply-adds fused one ulp
differently, which the step functions in translated shaders (`x >= 0`) turn
into whole regions of a different colour. With `-Gis` DXIL agrees with FXC at
least as closely as FXC's two builds agree with each other. The tool also
reports the share of output values the shaders wrote (82% for pixel, 20% for
vertex: most vertex shaders use a few of the 16 outputs), so a run whose
inputs never reached the outputs can't pass silently.

## Game-specific fixes

ReGPU stays game-neutral. A workaround for one game goes through the optional
hooks in `include/regpu/regpu.h`, not into the renderer:

| Hook | For |
| --- | --- |
| `rewriteShader` | Edit a shader's translated HLSL before it compiles, and tag it |
| `onDraw` | Set four `float4` constants (`vuser`) for draws with tagged shaders |
| `resolveWriteback` | Copy a resolved image back to memory because the game reads it on the CPU |
| `staticShaders`, `bootScreen` | Compile every shader in the game's files on first boot, with a progress screen |
| `onChar`, `onSwap` | Typed text, a callback per frame |

The plugin does not expose these to title projects yet; that belongs with the
title-profile work (ADR-009).

## Build options

| Option | Effect |
| --- | --- |
| `REGPU_FSR3` | AMD FSR 3. Needs `REGPU_FFX` set to the FidelityFX SDK's `Kits/FidelityFX` folder with its signed DLLs |
| `REGPU_DLSS` | NVIDIA DLSS. Needs `REGPU_DLSS_DIR` set to the DLSS SDK |
| `REGPU_FFX` | Also adds the `regpu-fsr1-bytecode` target, which rebuilds `src/regpu/gpu/bytecode/fsr1_cs.h` from `src/regpu/gpu/fsr1.hlsl` |

The default build needs none of them: the FSR 1, gamma and depth-copy compute
shaders are checked in as bytecode (built from FidelityFX SDK v2.1.0 headers
with the Windows SDK's FXC).

## Debugging

Start the game with `REGPU_DEBUG=1` and drive it with
`tools/regpu/regpuctl.py`: screenshots, frame captures, live shader edits and
`REGPU_*` switches. The method is in [ReGPU debugging](regpu-debugging.md).

## Credits and licences

- [ReGPU](https://github.com/zer0ducksgiven/ReGPU) by Scott (MIT): the renderer itself, written for
  [XenonRecomp](https://github.com/hedge-dev/XenonRecomp) ports and proven on Forza Motorsport 2.
- [Xenia](https://github.com/xenia-project/xenia) (BSD 3-Clause)'s research into the Xenos GPU informed this
  renderer. A few routines follow Xenia's closely: the PWL gamma conversion (`renderer.cpp`), the texture
  tiling and packed-mip offsets (`texture.cpp`). For those:

  > Copyright (c) 2015, Ben Vanik. All rights reserved.
  >
  > Redistribution and use in source and binary forms, with or without modification, are permitted
  > provided that the following conditions are met:
  > - Redistributions of source code must retain the above copyright notice, this list of conditions
  >   and the following disclaimer.
  > - Redistributions in binary form must reproduce the above copyright notice, this list of
  >   conditions and the following disclaimer in the documentation and/or other materials provided
  >   with the distribution.
  > - Neither the name of the project nor the names of its contributors may be used to endorse or
  >   promote products derived from this software without specific prior written permission.
  >
  > THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
  > IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  > FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  > CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  > DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  > DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  > IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  > OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
- AMD FidelityFX SDK (MIT); NVIDIA DLSS SDK (its own licence, only with `REGPU_DLSS`); Microsoft's
  `d3dcompiler_47.dll` (Windows SDK redistributable).

ReGPU is released under the [MIT licence](../src/regpu/LICENSE).
