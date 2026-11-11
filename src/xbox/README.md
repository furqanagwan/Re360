# Xbox

The Xbox 360's own interface for recompiled games. Today that is the Guide:
the menu that opens over a game, with achievements, settings, notifications,
the keyboard and Leave Game. It plays the console's own scenes instead of
redrawing them, following how Microsoft's Xbox backward compatibility shows
the Guide on Xbox on PC and the next-generation Xbox (Project Helix) app.

No Microsoft scenes, fonts, images or sounds are included. Each player's build
reads them from their own console's system update.

> Status: alpha. Used by [ReXGlue](https://github.com/furqanagwan/rexglue-sdk).

## Layout

| Folder | What it is | Depends on |
| --- | --- | --- |
| `ui/xui` | Reads and draws Xbox 360 XUI scenes with ImGui | `fmt`, `imgui` |
| `ui/guide` | The Guide: pages, achievements, notifications, settings | `ui/xui`, ReXGlue's runtime |
| `ui/dashboard` | Reserved for the dashboard; nothing yet | |

Each folder has `include/rex/ui/<part>`, `src` and `tests`. The include paths
(`<rex/ui/xui/...>`, `<rex/ui/guide/...>`) are the same as before the move.

The `xbox::ui_core` library is the XUI layer plus the Guide pieces that need no
runtime, and any recompilation project can use it. The full Guide currently
needs ReXGlue's services.

## Use it in ReXGlue

ReXGlue includes this repository as the `thirdparty/xbox` submodule. Clone
ReXGlue with `--recurse-submodules` and build it as usual. See
[Guide behavior and assets](docs/xbox-guide.md).

## Use the scene layer in another project

```cmake
set(XBOX_BUILD_CORE ON CACHE BOOL "" FORCE)
add_subdirectory(external/xbox)
target_link_libraries(my_game PRIVATE xbox::ui_core)
```

Parse a scene into `xui::Document`, build an `xui::Element` tree, then call
`xui::Render` on an ImGui draw list. Host-provided pages (message boxes, lists,
file pickers, downloads) are described in [host UI](docs/host-ui.md).

## Build and test on its own

From a Visual Studio x64 developer prompt:

```powershell
cmake -S . -B out/build -G "Ninja Multi-Config" -DCMAKE_CXX_COMPILER=clang++ -DXBOX_FETCH_DEPENDENCIES=ON
cmake --build out/build --config Release
ctest --test-dir out/build -C Release --output-on-failure
```

The XUI readers also have a libFuzzer target, built with AddressSanitizer:

```powershell
cmake -S . -B out/fuzz -G "Ninja Multi-Config" -DCMAKE_CXX_COMPILER=clang++ -DXBOX_FETCH_DEPENDENCIES=ON -DXBOX_BUILD_TESTS=OFF -DXBOX_BUILD_FUZZERS=ON
cmake --build out/fuzz --config Release
ctest --test-dir out/fuzz -C Release --output-on-failure -L fuzz
```

`xbox.fuzz.xui` runs for `XBOX_FUZZ_SECONDS` (60 by default).

## Contributing

Work on a branch and open a pull request; `main` only changes through reviewed
PRs with a passing build. Guide and dashboard issues live here; runtime and
input issues live in ReXGlue. Where the code came from is in
[provenance](docs/provenance.json).

## License

BSD-3-Clause. Not affiliated with or endorsed by Microsoft or Xbox.
