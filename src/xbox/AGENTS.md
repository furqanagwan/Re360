# Xbox development

Read README.md, docs/extraction.md, docs/xbox-guide.md and the assigned issue.
This repository owns the Xbox 360 interface sources: the XUI scene runtime
(`ui/xui`), the Guide (`ui/guide`) and, later, the dashboard (`ui/dashboard`).
ReXGlue is a consumer and provides the current complete host adapter. Keep the
standalone `xbox::ui_core` target free of ReXGlue runtime dependencies. Keep
public include paths (`<rex/ui/xui/...>`, `<rex/ui/guide/...>`) and guest
behavior stable. Introduce further host interfaces in small, tested changes
rather than removing features to compile.

Use Windows x64, Clang 18 or newer, CMake 3.25 or newer and Ninja. Run standalone
Debug/Release builds and CTest, and the affected ReXGlue unit suite for adapter
changes. Zero tests is not a pass. Real scene/title tests require private
owner-supplied assets; report absent assets and hardware as blocked.

Do not commit console updates, extracted assets, game data or restricted SDK
material. Preserve copyright notices and licenses. Do not introduce CPU JIT
or title-ID hacks. Source provenance is pinned in docs/provenance.json.

Code has no comments apart from licence headers; why-notes go in `research/`,
one Markdown file per topic. Follow .clang-format and .editorconfig. Work on
focused branches, preserve user edits and never force-push. Record validation
and limitations. Only close issues when their tests and acceptance criteria
are met.
