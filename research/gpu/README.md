# GPU research

How the Xbox 360's Xenos GPU behaves and how ReXGlue maps it to Direct3D 12.
Notes move here from code comments as each part of `src/graphics` is cleaned
up (see the [audit plan](../audit/2026-10-09-codebase-audit.md)).

Topics:

- [Asynchronous pipeline creation](async-pipeline-creation.md)
- [k_DXT3A_AS_1_1_1_1 and resolve copy command 2](dxt3a-as-1111.md)
- [Predicated tiling: window offsets in EDRAM bases](window-offset-edram.md)
- [Xenos MSAA sample positions](xenos-sample-positions.md)
- [Shader operand components](shader-operand-components.md)
- [Wide 1D textures](wide-1d-textures.md)
- [What ReGPU's renderer had to get right](regpu-xenos-notes.md) (`src/regpu/gpu`)

Existing records still in `docs/`:

- [Edge GPU review, 2026-10-07](../../docs/edge-gpu-review-20261007.md)
- [DXIL shader toolchain](../../docs/shader-dxil.md)
- [Shader cache](../../docs/shader-cache.md) and [replacements](../../docs/shader-replacements.md)
- [Native renderers](../../docs/native-renderers.md)
