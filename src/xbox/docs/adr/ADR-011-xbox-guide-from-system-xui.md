# ADR-011: The Xbox guide runs the console's own XUI scenes

Date: 2026-09-30. Status: implemented (format layer, runtime, guide); owner pad session pending in
[RG-GDK-041](https://github.com/furqanagwan/xbox/issues/1).

## Context

Recompiled titles have no Xbox guide. On the console, pressing the Guide button in a
game opens XAM's Metro guide: its tab blades, focus highlight, sounds, the
achievements view, and "Xbox Home" with an exit confirmation. Owners want the same
over a recompiled title, opened with Back + Start.

On the console, the guide's look and motion are data. Dashboard 2.0.17559's system
update holds `hud.xex`, `huduiskin.xex`, `xam.xex` and `gamerprofile.xex`. Their
resources are XUIZ packages of XUR v8 scenes (`GuideMain.xur`, the tab scenes,
`skin.xur`, `hudbkgnd.xur`, `802_Achievements.xur`), string tables and XMA sounds.
Only the behaviour is code: the scene classes' overrides in `hud.xex`.

## Decision

The SDK ships a native XUI runtime that loads those scenes at run time from the
owner's own system update, named by a cvar. It covers a XUR v8 reader, a class schema,
skin visuals, named-frame timelines, rendering on an ImGui draw list, and XMA UI
sounds through XAudio2. The guide's behaviour is native C++ in place of the `hud.xex`
class overrides: tab and focus navigation, achievements from `AchievementManager`,
and the exit confirmation. While the guide is open the guest sees XN_SYS_UI and a
neutral pad, as for XAM dialogs.

No Microsoft data is in the repository: no scenes, images, sounds, strings or XDK
definition files. The schema holds only what decoding needs: for each class the
guide's scenes use, its base class and its properties' names, types and order.
That order is interoperability data. It is checked by decoding every guide, skin
and achievement scene to its declared object count, with no bytes left over. XUIHelper
(GPL-3.0) served as the format reference, and no code was taken from it.

Without a system update the guide says it is unavailable and nothing else changes.

## Alternatives rejected

- **A hand-built look-alike** (DashX360's approach). It drifts from the console, and
  the dashboard's hundreds of timeline keyframes would be copied by hand.
- **Shipping extracted assets or scenes.** Not ours to distribute
  (see AGENTS.md).
- **Running `hud.xex` itself.** It is XAM-internal code built against the whole
  kernel UI stack; ADR-004 keeps system code out of the recompilation boundary.
- **Converting scenes offline with XUIHelper.** It adds a .NET GPL tool to the
  owner's setup for what a 600-line reader does.

## Limits

These values are inferred from the scenes and observed timings, not from any
specification: the XUI text-style bits, the ease curve and the 60 frames/s timeline
rate. The console's `.xtt` fonts are encrypted, so Segoe UI stands in for Segoe
Xbox. Xbox Live features (friends, party, messages) are not implemented. The
schema covers the guide's classes only: other scenes may name classes it lacks,
and the reader rejects those rather than guess.
