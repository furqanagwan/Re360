# Fuzzing and AddressSanitizer

The file parsers take untrusted bytes from disc images, packages and
executables. Each one has a libFuzzer target, and the whole SDK can be built
with AddressSanitizer (ASan). Both need LLVM Clang's compiler-rt, which the
Windows LLVM installer includes.

ASan on Windows only works with the release C runtime, so build the `Release`
(or `RelWithDebInfo`) configuration of these presets. Visual Studio ships its
own `clang_rt.asan_dynamic-x86_64.dll`, which does not match LLVM Clang's, so
configuring copies the compiler's copy next to the built programs.

## Fuzz targets

| Target | Parser | Entry point |
| --- | --- | --- |
| `xex_fuzzer` | XEX2 headers, resources and image loading | `XexSourceTitleId`, `XexTitleName`, `XexModule::Load` |
| `xdbf_fuzzer` | XDBF title data (SPA) | `XdbfGameData` |
| `xdvdfs_fuzzer` | ISO / XDVDFS disc images | `DiscImageDevice` |
| `stfs_fuzzer` | STFS and SVOD packages | `StfsContainerDevice` |
| `xui_fuzzer` | XUIZ packages, XUIS string tables, XUR scenes | `Package::Parse`, `ParseStringTable`, `ParseXur` |

`xui_fuzzer` lives with the XUI sources in `src/xbox/ui/xui/fuzz`.

`fuzz_seeds` and `xui_fuzz_seeds` write small synthetic seed inputs, built the
same way as the unit tests. No game data is used or needed.

## Running

```powershell
cmake --preset win-amd64-gdk-fuzz
cmake --build --preset win-amd64-gdk-fuzz-release
ctest --preset win-amd64-gdk-fuzz-release --output-on-failure
```

The build preset builds only the `fuzzers` target. Each `fuzz.<parser>` test
runs for `REXGLUE_FUZZ_SECONDS` (60 by default). New inputs are saved to `out/build/win-amd64-gdk-fuzz/tests/fuzz/corpus/<parser>`.
Crashing inputs are saved to `artifacts/<parser>`. To run one target longer:

```powershell
out\win-amd64-gdk-fuzz\Release\xdvdfs_fuzzer.exe -max_total_time=3600 `
  out\build\win-amd64-gdk-fuzz\tests\fuzz\corpus\xdvdfs `
  out\build\win-amd64-gdk-fuzz\tests\fuzz\seeds\xdvdfs
```

To check a crash, pass the saved input to the target. A fix comes with a
unit test built from that input.

## AddressSanitizer build

```powershell
cmake --preset win-amd64-gdk-asan
cmake --build --preset win-amd64-gdk-asan-release --target unit_tests
ctest --preset win-amd64-gdk-asan-release -L unit --output-on-failure
```

## Continuous integration

The `AddressSanitizer and parser fuzzing` job in
`.github/workflows/gdk-validation.yaml` runs the ASan unit tests and then
each fuzz target for 30 seconds on every push and pull request.

## Results

Recorded runs and the bugs they found are in
[research/io/parser-fuzzing.md](../research/io/parser-fuzzing.md).
