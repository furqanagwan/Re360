# CPU research

How the Xbox 360's Xenon PowerPC CPU behaves and how the recompiler
translates it: instruction semantics, FPSCR and VSCR state, MSR and interrupt
locks, and code discovery. Notes move here from code comments as
`src/codegen` and `include/rex/ppc` are cleaned up (see the
[audit plan](../audit/2026-10-09-codebase-audit.md)).

Topics:

- [Memory barriers: sync, lwsync, eieio](memory-barriers.md)

Existing records still in `docs/`:

- [PowerPC reference manuals](../../docs/ppc/)
- [Indirect function discovery](../../docs/indirect-function-discovery.md)
- [CRT setjmp/longjmp](../../docs/crt-nonlocal-jumps.md)
- [Threading contracts](../../docs/threading-contracts.md)
