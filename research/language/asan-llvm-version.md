# AddressSanitizer needs LLVM newer than 20.1.8

With LLVM 20.1.8, the version GitHub's Windows runner had preinstalled, four
unit tests crashed under AddressSanitizer on every CI run from the repository's
first one (2026-11-10). They pass with LLVM 22.1.8, so the sanitizer job in
`.github/workflows/gdk-validation.yaml` installs that version.

## The four tests

All four throw a C++ exception and catch it, three of them across
`rexruntime.dll`:

- `C++ exceptions unwind through rexruntime under the GDK toolchain`
  (`tests/unit/gdk/gdk_runtime_test.cpp`): `unknown-crash`, a read through a
  wild pointer in memory ASan marks as internal (shadow `fe`).
- `Game source extraction commits a matching folder and preserves existing data`:
  access violation at `0xa` inside `rex::system::ExtractGameSource`, under the
  CRT's exception dispatch (`VCRUNTIME140`, `ntdll`).
- `Expired and foreign-thread guest jump buffers are rejected`: null read in
  Catch2's `translateActiveException` while matching a thrown message.
- `cvar LoadConfig reads a file saved with unescaped Windows paths`: access
  violation at `0x8` in `rex::cvar::LoadConfig`.

The same four failed on `main` and on every pull request, so the job was red
for a toolchain reason and could hide a real new failure.

## Measured

| Where | LLVM | Result |
| --- | --- | --- |
| GitHub `windows-2025-vs2026` runner, `win-amd64-gdk-asan-release` | 20.1.8 | 4 of 617 unit tests fail (run 43002227080) |
| Local, Windows 11, same preset and commit | 22.1.8 | the 4 pass |

Not measured: LLVM 21, and 20.1.8 on the local machine, so the fix is shown
to be the newer LLVM only by the CI run that uses it. If 22.1.8 ever fails the
same way on the runner, the next suspect is the runner image, not the code.
The other CI jobs build without ASan and pass on 20.1.8; they are not pinned.
