# ReGPU: the `regpu` GPU plugin

ReGPU is Re360's second Xenos renderer. It reads the same PM4 command stream as
the default `xenos` plugin but keeps render targets as host textures, turns
resolves into copies, and translates shader microcode to readable HLSL. It can
upscale (FSR 1 built in; FSR 3 and DLSS as build options).

It builds, loads through the plugin interface and runs a command stream on a
real Direct3D 12 device (`gpu.regpu_plugin_smoke`). **It has not been run on a
game inside Re360 yet.** Before this merge it rendered Forza Motorsport 2
end to end as a XenonRecomp port; getting the three 007 titles to gameplay on
it is the next gate in the [native GPU plan](native-gpu-plan.md).

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
ReGPU's register file, and presents into the game window the runtime already
owns. The kernel's video exports (`src/kernel/xboxkrnl/xboxkrnl_video.cpp`) are
shared by both plugins; ReGPU's own copies were removed in the merge.

## What it does not do yet

- **Guide and overlays.** ReGPU presents with its own swap chain, so the Xbox
  Guide, achievement toasts and the debug overlays, which draw through the
  runtime's presenter, do not appear with `gpu_plugin=regpu`.
- **Runtime settings.** `vsync`, `resolution_scale` and the other GPU cvars
  belong to the `xenos` plugin. ReGPU starts at the game window's size with
  vsync on and a 60 fps cap, and takes its switches from `REGPU_*` environment
  variables.
- **1280x720 guests only.** The renderer assumes the game draws at 1280x720
  (see the [Xenos notes](../research/gpu/regpu-xenos-notes.md)).
- **No tessellation; memexport writes are dropped.**
- **Shaders compile on the player's PC** with FXC (`d3dcompiler_47.dll`), not
  at build time.
- **One renderer per process, and no teardown.** Its threads run until the
  process exits, which is how titles exit today.
- **Its own support code.** ReGPU still carries its own logger, PNG writer,
  live switches and D3D12 device setup instead of the runtime's. They are
  small, and replacing them is safe only once a game runs on it here.
- **The debug server's controller** (`regpuctl.py press`) is not merged into
  the runtime's input yet.

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
