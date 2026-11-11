# Memory barriers: sync, lwsync, eieio

Generated code turns the guest's barrier instructions into C++ fences: `sync` and
`eieio` into `std::atomic_thread_fence(std::memory_order_seq_cst)`, `lwsync` into
`std::memory_order_acq_rel`. Before 2026-11-11 all three generated nothing. Code:
`BuildSync`, `BuildLwsync` and `BuildEieio` in `src/codegen/builders/system.cpp`.

## Why they are needed on x86-64

The comment that used to stand there said x86 has strong ordering, so a barrier
is a no-op. That is true of two of the four orderings, and not of the compiler:

- **The compiler reorders across a missing fence.** Recompiled guest memory
  accesses are plain C++ loads and stores. With nothing between them, Clang may
  move, merge or drop them across the point where the guest placed its barrier.
  A guest lock that publishes data, `lwsync`, then sets a flag can have the flag
  store hoisted above the data. Any fence stops that.
- **x86 reorders a store before a later load (StoreLoad).** Total store order
  keeps loads after loads, stores after stores and loads before stores in program
  order, but a store can still wait in the store buffer while a later load reads
  memory. Guest code that relies on `sync` for this (Dekker-style flags between
  two threads) needs `mfence`, which `seq_cst` emits.
- **`lwsync` is acquire plus release.** It orders everything except StoreLoad,
  which is exactly `acq_rel`. On x86-64 that costs no instruction; it only stops
  the compiler.
- **`eieio` orders I/O.** The host has no separate I/O space, so it takes the
  conservative `seq_cst`. Guest MMIO writes (GPU registers) are followed by
  `eieio`, which is also how codegen recognises them (see the builder context
  notes).

`isync` stays empty: it orders instruction fetch, and recompiled code fetches
nothing at run time.

## Source and measurement

Upstream ReXGlue `bd833a2` (Carlos Estrague, 2026-10-01, released in v0.10.1).

Cost, measured on James Bond 007: Blood Stone (Release, ReGPU, fullscreen
3840x2160, NVIDIA RTX 5080 Laptop GPU), in its Istanbul level, two runs each,
fps sampled every 2 s for 20 s and the process's CPU time over the same 20 s:

| Build | fps | CPU cores busy |
| --- | --- | --- |
| No fences | 30 (the game's cap) | 3.6, 3.7 |
| Fences (6,766 in the generated code) | 30 (the game's cap) | 3.6, 3.6 |

No measurable cost at the game's cap.

Not measured: Quantum of Solace and 007 Legends (no game files on the machine
at the time), and an uncapped title where the fences' cost would show as
frame rate.
