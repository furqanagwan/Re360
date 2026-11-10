# ADR-018: Re360 is one repository with two GPU plugins

Date: 2026-11-10. Status: accepted owner direction. Implemented: the merge, the
`rexgpu-regpu` plugin and its smoke test. Not yet done: a title running on
`regpu`, and everything in the later phases of the
[native GPU plan](../native-gpu-plan.md).

## Context

The recompiler and runtime (ReXGlue, called ReCPU in planning) and the lean
Xenos renderer (ReGPU) lived in two repositories. The research behind the
native GPU plan recommended keeping them apart and consuming ReGPU as a pinned
submodule. The owner decided instead to combine them into one project, Re360,
so that someone can pick up a game with a single checkout, with no code kept
twice.

[Native renderers](../native-renderers.md) asked for an architecture decision
before any native-renderer work. This is that decision, widened to all three
levels the plan describes.

## Decision

1. **One repository.** ReGPU's history is merged into this one. Its renderer
   lives in `src/regpu`, its API in `include/regpu`, its tools in `tools/regpu`.
   There is no ReGPU submodule.
2. **Two GPU plugins behind one interface.** `rexgpu-xenos` stays the default
   and the reference image. `rexgpu-regpu` is opt-in with `gpu_plugin=regpu`.
   Both implement `IGraphicsSystem`; a run uses one or the other, never both.
3. **The kernel owns the video exports.** ReGPU's XenonRecomp guest ABI (its own
   `Vd*` exports and a register file in guest memory) is removed. ReGPU is
   reached only through `regpu::driver`.
4. **Three levels of "native GPU", in order.** Level 1: shaders translated ahead
   of time. Level 2: the lean host renderer (ReGPU). Both are generic and are
   SDK work. Level 3: a renderer that hooks a game's own Direct3D calls. That is
   per-game work and lives in title repositories (ADR-005, ADR-009).
5. **HLSL is allowed for ReGPU.** ADR-008 rejected generating HLSL and compiling
   it with DXC for `rexgpu-xenos`. That rejection stands for `rexgpu-xenos` and
   does not apply to ReGPU, whose translator already emits HLSL. Which
   translator feeds ahead-of-time shaders is decided on Phase 2 measurements.
6. **Names.** The project is Re360. The CLI, CMake package, cvars and C++
   namespace keep the names `rexglue` and `rex::`, so title projects build
   unchanged. Renaming them is a separate, breaking change.
7. **Plugin ABI 2.** `IGraphicsSystem` gains `SetHostWindow` (a plugin with its
   own presenter draws into the title window) and `FlushCaches` (called before a
   title exits).

## Consequences

- ReGPU keeps its own code style and comments inside `src/regpu` and
  `include/regpu`; clang-format is disabled there so the code stays comparable
  with its origin. The plugin glue in `src/regpu/plugin` follows the SDK's rules.
- ReGPU is MIT-licensed code inside a BSD 3-Clause repository. Its licence file
  stays at `src/regpu/LICENSE` and both are named in the README.
- Two Xenos renderers are maintained until Phase 2 measures them against each
  other. They share the kernel exports and the plugin interface, not their
  internals: each has its own command processor, texture decoding and shader
  translator.
- ReGPU still has its own logger, PNG writer, live switches and device setup.
  Moving it onto the runtime's is deferred until a title runs on it, so that
  each replacement can be checked against a picture.
- Issue numbers in commit messages and documents (`#304`, RG-GDK-*) refer to
  [furqanagwan/rexglue-sdk](https://github.com/furqanagwan/rexglue-sdk), where
  that history's issues live.

## Rejected alternatives

- **Separate repositories with a submodule.** Fewer moving parts per repository,
  but two checkouts to keep in step and a duplicate set of kernel exports.
- **Replace `rexgpu-xenos` with ReGPU now.** ReGPU has been proven on one game,
  at 1280x720, under another recompiler. The three 007 titles have recorded
  baselines only on `xenos`.
- **Fold ReGPU's code into `src/graphics`.** The two renderers share no
  internals; interleaving them would hide which one a fix applies to.
