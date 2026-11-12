# XboxGPU debugging

How to find and fix a rendering bug with `gpu_plugin=regpu`. What XboxGPU is and
how it is wired into Re360 is in [XboxGPU](regpu.md); what the renderer had to get
right is in the [Xenos notes](../research/gpu/regpu-xenos-notes.md), which you
should read before changing `src/regpu/gpu`.

## Ground rules

- **Keep XboxGPU game-neutral.** A fix for one game goes in that game's project,
  through the hooks in `include/regpu/regpu.h` (`rewriteShader`, `onDraw`,
  `resolveWriteback`, ...). Change XboxGPU itself only for console behaviour every
  game would see, and explain it in Xenos terms, not in terms of one game's
  scene. If a fix needs something the hooks can't express, add a general hook.
- **Measure, don't guess.** Rendering bugs here are almost always one register,
  format or EDRAM detail. Find the draw, read its state, change one thing live,
  compare.
- **Compare against references.** The `xenos` plugin, Xenia and a real console
  (XBDM, below) show what the game should look like. If the reference renders
  the same artefact, it may be console-accurate: check the console before
  "fixing" it.
- **Name the switch.** New behaviour that could be wrong for some game gets a
  `REGPU_*` live switch (`static LiveSwitch x("REGPU_NO_...")`) so it can be
  A/B tested without a rebuild.

## Bringing up a game on XboxGPU

1. Get the game running on the default `xenos` plugin first; that is the
   reference image.
2. Run it with `--gpu_plugin=regpu`. Expect a first boot that compiles shaders
   for minutes (FXC). Later boots load them from the title's cache folder.
   Changing the preamble in `src/regpu/gpu/hlsl.cpp` invalidates every cached
   shader.
3. Work through the screens. For each broken one: capture, find the draw, fix,
   re-check the others.
4. Write the game's quirks down in the game's project, not here.

## The debug loop

Start the game with the debug server, then drive it without restarting:

```bash
REGPU_GAME=path/to/game.exe py -3 tools/regpu/regpuctl.py start   # the game set to gpu_plugin=regpu
py -3 tools/regpu/regpuctl.py shot out/now.png          # what's on screen
py -3 tools/regpu/regpuctl.py capture                   # trace the next frame: draws, textures, resolves
py -3 tools/regpu/regpuctl.py ps <hash> tint 1,0,0      # paint one pixel shader red: which draw is this?
py -3 tools/regpu/regpuctl.py ps <hash> "float4(r[4].xyz, 1)"   # show an intermediate value
py -3 tools/regpu/regpuctl.py vs <hash> "o[14] = VFetchK(7, float(vindex));"  # pass a vertex input out
py -3 tools/regpu/regpuctl.py switch REGPU_NO_ANISO 1   # A/B a behaviour live
```

The full command list is at the top of `tools/regpu/regpuctl.py`. A capture writes the draw list to the log
(`draw #N vs ... ps ... t0=...`), each bound texture to `out/capture/tex`, and every resolved image to
`out/res`.

Finding what draws a broken pixel:

1. `shot`, locate the pixel.
2. Tint shader candidates one at a time (or in colours) and re-measure **that pixel**. Pick a pixel
   away from overlays (HUD, menus, reticles): a full-screen or UI shader over your pixel answers every
   test the same way.
3. Once you have the draw: `hlsl <hash>` to read its translated shader, then `ps`/`vs` edits to show
   each term (`float4(r[n].xyz * k, 1)`), until the first wrong one.

## Traps that cost time before

- **Screenshots are tone-mapped.** Pixel values are not shader outputs; compare relative values or
  scale terms (`* 100`) to see small ones.
- **A shader edit with a type error silently drops the draw.** `vs` statements must assign a full
  `float4` (`o[6] = float4(v.xyz, 1);`). If everything you try looks identical, check this.
- **Check a dump is fresh.** A run that crashed or quit early leaves the previous `out/` files in place.
  Compare file times with the run.
- **Captured textures are per first use.** The textures list shows each address once; the draw lines in
  the log show every binding.
- **Hashes key everything.** Shaders are identified by the hash of their used microcode (the debug tools
  print it). The game patches vertex-fetch fields into copies at load; those copies share their base
  shader's translation.
- **Measure an area, not one number, only when the area is clean.** A region that also covers other
  objects moves for reasons unrelated to your test.

## Comparing with real hardware

With a modded console running XBDM (port 730):

```bash
py -3 tools/regpu/xbdm.py --host 192.168.x.x info          # console, running title
py -3 tools/regpu/xbdm.py screenshot out/xbox.raw           # the framebuffer, raw
py -3 tools/regpu/xbox_fb.py out/xbox.raw out/xbox.png      # untiled, BGRA -> PNG
py -3 tools/regpu/xbdm.py peek 82000000 64                  # guest memory
```

Same scene, same settings, both sides: if the console shows it too, it isn't a XboxGPU bug.

## Layout

| Path | What |
|---|---|
| `include/regpu/regpu.h` | The API: Config, Host, hooks, Start |
| `include/regpu/driver.h` | What the kernel's `Vd*` exports reach |
| `src/regpu/plugin/graphics_system.cpp` | The GPU plugin: XboxGPU's host inside Re360 |
| `src/regpu/gpu/gpu.cpp` | Command processor (PM4 packets), interrupts, vblank, frame cap |
| `src/regpu/gpu/ucode.cpp` | Xenos shader microcode decoder |
| `src/regpu/gpu/hlsl.cpp` | Microcode to HLSL (the preamble: fetches, unpacking, constants) |
| `src/regpu/gpu/renderer.cpp` | D3D12: draws, EDRAM surfaces, resolves, textures, pipelines, upscaling, captures |
| `src/regpu/gpu/texture.cpp` | Texture formats, tiling, decoding |
| `src/regpu/gpu/frame.cpp`, `src/regpu/gpu/shader_scan.cpp` | Shader uploads, finding shaders in memory ahead of use |
| `src/regpu/render/presenter.cpp` | Swap chain, frame pacing |
| `src/regpu/debug.cpp` | The debug server (`tools/regpu/regpuctl.py`) |
| `tools/regpu` | regpuctl, XBDM tools, shader disassembler (`tools/regpu/xenos_dis.cpp`, built as `regpu_dis`), frame comparison |
