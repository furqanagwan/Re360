# Committing guest pages inside a GPU-opened heap

A guest commit of pages that are already committed is treated as done, even
when the host refuses the requested protection: `rex::memory::AllocFixed`
(`src/core/memory_win.cpp`) checks the range with `VirtualQuery` when
`VirtualAlloc(MEM_COMMIT)` fails, applies the protection if it can, and returns
the address either way.

## Why

ReGPU opens a Direct3D 12 heap over the guest's 512 MB physical view at
`0xA0000000` (`MapGuestMemory`, `src/regpu/gpu/renderer.cpp`) so the GPU reads
vertex data in place. Pages in such a heap stay committed read-write: Windows
refuses to make them no-access (ReGPU already lifts no-access pages to
read-write while it opens the heap).

Every title calls `VdPersistDisplay` at boot, which commits a 64-byte block
with no-access protection in the physical heap
(`src/kernel/xboxkrnl/xboxkrnl_video.cpp`). Under ReGPU that commit failed
(`VirtualAlloc` error 87; the pages: `MEM_COMMIT`, `PAGE_READWRITE`,
`MEM_MAPPED`) and the guest allocation with it, logged as
`BaseHeap::AllocFixed failed to alloc range from host` /
`PhysicalHeap::Alloc unable to pin physical memory` at every boot of all three
007 titles (2026-11-11). Under the `xenos` plugin the commit succeeds.

## Cost

The block stays read-write instead of no-access, so a stray guest access to it
reads or writes memory instead of faulting. The memory itself is committed and
usable, which is what the allocation needs. `BaseHeap::AllocFixed`'s error now
also reports the request and the host pages' state, for the next such case.

Measured: Blood Stone and 007 Legends boot on ReGPU with no error lines;
2,234 unit and PPC tests pass.
