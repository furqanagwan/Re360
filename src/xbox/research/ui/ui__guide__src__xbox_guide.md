# Xbox guide: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/xbox_guide.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 59

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L59)

```text
// Entries taken out of the guide for recompiled titles; the scenes still
```

## Source note 2, line 60

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L60)

```text
// have them. Kept as a list so one can be put back by deleting its line.
```

## Source note 3, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L62)

```text
// Settings > Family Settings
```

## Source note 4, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L63)

```text
// Settings > Account Management
```

## Source note 5, line 64

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L64)

```text
// Settings > Kinect Tuner
```

## Source note 6, line 65

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L65)

```text
// Settings > Turn Off Console
```

## Source note 7, line 66

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L66)

```text
// Home > Connect to Xbox Live
```

## Source note 8, line 67

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L67)

```text
// Home > the title, shown disabled
```

## Source note 9, line 70

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L70)

```text
// Home's Xbox Home entry and the Y button end the title, worded as the Xbox
```

## Source note 10, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L71)

```text
// One and Series consoles' guide for 360 titles words them: a recompiled title
```

## Source note 11, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L72)

```text
// has no dashboard to return to.
```

## Source note 12, line 76

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L76)

```text
// Settings > System Settings opens the console's own settings; the Series
```

## Source note 13, line 77

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L77)

```text
// consoles' guide calls that entry Xbox One X Settings.
```

## Source note 14, line 80

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L80)

```text
// The Xbox One and Series consoles' guide titles the Home tab with the
```

## Source note 15, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L81)

```text
// player's gamertag.
```

## Source note 16, line 84

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L84)

```text
// The tabs left to right. The backward-compatibility guide has three; in
```

## Source note 17, line 85

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L85)

```text
// 2.0.17559's, Media (3) is removed and GuideMain's timelines are rewritten to
```

## Source note 18, line 86

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L86)

```text
// match (UseThreeTabs).
```

## Source note 19, line 91

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L91)

```text
// The legend button glyphs (sharedres A-Button.png to Y-Button.png: 18 x 18,
```

## Source note 20, line 92

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L92)

```text
// a flat disc of radius 8 centred at (9, 9)) drawn as discs with their letter
```

## Source note 21, line 93

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L93)

```text
// centred, so they are sharp at 4K: the console's only copies are 18 pixels.
```

## Source note 22, line 106

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L106)

```text
// The console darkens the title behind the guide to about a quarter of its
```

## Source note 23, line 107

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L107)

```text
// brightness (measured from a capture of dashboard 2.0.17559). XAM does that
```

## Source note 24, line 108

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L108)

```text
// in code: no scene draws it. It fades over the backdrop's ClosedToFull
```

## Source note 25, line 109

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L109)

```text
// (frames 99 to 122) and FullToClosed (123 to 139) animations.
```

## Source note 26, line 118

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L118)

```text
// XDBF achievement flag: shown before it is earned. Without it, secret.
```

## Source note 27, line 120

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L120)

```text
// The default gamer picture, in XAM's shared resources.
```

## Source note 28, line 133

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L133)

```text
// XUIS strings use FormatMessage inserts: %1!u! and %2!u!.
```

## Source note 29, line 151

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L151)

```text
// The ring of light's lit quadrant, as the console draws player 1's
```

## Source note 30, line 152

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L152)

```text
// (measured from a capture of dashboard 2.0.17559).
```

## Source note 31, line 155

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L155)

```text
// A copy of `figure`'s fill with every gradient stop recoloured to `rgb`,
```

## Source note 32, line 156

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L156)

```text
// keeping each stop's alpha.
```

## Source note 33, line 187

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L187)

```text
// The controller battery icons (Controller_OneFourth.xur to Controller_Full.xur,
```

## Source note 34, line 188

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L188)

```text
// each a 38 x 14 scene of one PNG, ico_32x_Ctrl-Battery1 to 4) redrawn as
```

## Source note 35, line 189

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L189)

```text
// shapes traced from those PNGs, so they are sharp at 4K: the console's only
```

## Source note 36, line 190

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L190)

```text
// copies are 38 x 14 pixels. Units are the PNG's pixels.
```

## Source note 37, line 213

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L213)

```text
// The pad: bumpers, body and grips, one outline so nothing overlaps.
```

## Source note 38, line 225

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L225)

```text
// Without the anti-aliasing fringe: at the grips' sharp corners it throws
```

## Source note 39, line 226

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L226)

```text
// thin spikes across the body.
```

## Source note 40, line 231

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L231)

```text
// The Guide button, darker on the body.
```

## Source note 41, line 236

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L236)

```text
// The battery: a faint body, its frame, the terminal and the bars.
```

## Source note 42, line 249

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L249)

```text
// A legend button glyph (kButtonGlyphs) as a disc with its letter, bold, the
```

## Source note 43, line 250

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L250)

```text
// centre of its ink on the disc's centre. Units are the PNG's pixels.
```

## Source note 44, line 261

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L261)

```text
// pixels per unit
```

## Source note 45, line 276

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L276)

```text
// At the screen size of the disc (pixels per PNG unit times 11).
```

## Source note 46, line 283

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L283)

```text
// The centre of its ink on the disc's centre, placed at the exact position:
```

## Source note 47, line 284

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L284)

```text
// AddText truncates to whole display units, three pixels each at 300%.
```

## Source note 48, line 293

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L293)

```text
// The tab label column (Blade_Focus) starts a unit inside the centre blade's
```

## Source note 49, line 294

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L294)

```text
// top and left edges. At 720p that is under a pixel; at 4K it showed as a
```

## Source note 50, line 295

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L295)

```text
// white line above and beside the column.
```

## Source note 51, line 306

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L306)

```text
// The letters the legend groups and visuals lay over the button glyphs, for
```

## Source note 52, line 307

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L307)

```text
// the console's font; DrawButtonGlyph draws them instead.
```

## Source note 53, line 326

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L326)

```text
// The battery icon's frame for a charge: GuideMain names frames 0 to 3
```

## Source note 54, line 327

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L327)

```text
// Little, Low, Medium and High (Controller_OneFourth.xur to
```

## Source note 55, line 328

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L328)

```text
// Controller_Full.xur). XInput's four levels arrive as 5, 30, 60 and 100.
```

## Source note 56, line 339

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L339)

```text
// The gamerscore glyph (sharedres GScore_white.png, 32 x 32) traced at a
```

## Source note 57, line 340

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L340)

```text
// larger size: a white disc of radius 14.5 with a G cut out of it. The G is a
```

## Source note 58, line 341

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L341)

```text
// ring gap (radius 7 to 9.3) open at the upper right, a crossbar and a stem
```

## Source note 59, line 342

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L342)

```text
// down to the ring. Units are the PNG's pixels, its centre at (16, 16).
```

## Source note 60, line 351

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L351)

```text
// The crossbar and the stem.
```

## Source note 61, line 356

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L356)

```text
// The ring gap, except the mouth from the crossbar up to 70 degrees.
```

## Source note 62, line 383

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L383)

```text
// The status icons by the clock, which XAM sets in code: the controller's
```

## Source note 63, line 384

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L384)

```text
// battery (XboxGuide::UpdateControllerBattery) and the ring of light with
```

## Source note 64, line 385

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L385)

```text
// player 1's quadrant lit.
```

## Source note 65, line 387

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L387)

```text
// The header's ring, a child of the scene; the Sign In button's visual
```

## Source note 66, line 388

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L388)

```text
// has another.
```

## Source note 67, line 398

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L398)

```text
// The visual's own light1 to light4 are the lit quadrants, over the dim
```

## Source note 68, line 399

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L399)

```text
// ROL_Off ring.
```

## Source note 69, line 463

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L463)

```text
// An exact match first: "Yes" is also the start of "Yes, apply it".
```

## Source note 70, line 536

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L536)

```text
// A BC asset folder must not silently replace a 360 title's Guide.
```

## Source note 71, line 574

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L574)

```text
// Preferences: the emulator's own list where there is one (Family Timer
```

## Source note 72, line 575

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L575)

```text
// hidden).
```

## Source note 73, line 595

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L595)

```text
// Latin with Extended-A and -B, Greek, Cyrillic, punctuation, euro and
```

## Source note 74, line 596

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L596)

```text
// trade mark, plus every character of the guide's string tables (filled in
```

## Source note 75, line 597

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L597)

```text
// below once the bundle is read). Glyphs the font lacks are left out.
```

## Source note 76, line 600

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L600)

```text
// The atlas reads the ranges when it builds, so they outlive this call.
```

## Source note 77, line 607

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L607)

```text
// One size; the static atlas scales it to each XUI point size. Baked for
```

## Source note 78, line 608

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L608)

```text
// the guide's larger text (20 pt) at the display's scale over the
```

## Source note 79, line 609

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L609)

```text
// console's 480-line scenes: 64 px at 1080p, 120 px at 2160p.
```

## Source note 80, line 616

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L616)

```text
// The console's own font, from the guide built into the title.
```

## Source note 81, line 620

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L620)

```text
// The characters the guide's own strings use.
```

## Source note 82, line 641

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L641)

```text
// The atlas owns and frees the copy.
```

## Source note 83, line 652

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L652)

```text
// The console guide draws all its text in the one system font.
```

## Source note 84, line 656

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L656)

```text
// Without the console's font: Segoe UI, the host's stand-in for it.
```

## Source note 85, line 691

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L691)

```text
// dlc://<media ID>/banner or /tile: art from the built-in add-on catalogue.
```

## Source note 86, line 778

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L778)

```text
// The HUD frame, with the guide as its hosted app.
```

## Source note 87, line 793

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L793)

```text
// The title sees system UI, as for the Guide button on the console.
```

## Source note 88, line 802

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L802)

```text
// GuideMain's "<tab>Close" brings a tab's blade in (the guide opening, or
```

## Source note 89, line 803

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L803)

```text
// an app launched from it closing); "<tab>Open" takes it out.
```

## Source note 90, line 807

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L807)

```text
/*initial=*/
```

## Source note 91, line 822

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L822)

```text
// Controls the guide acts on; the rest stay in the menu, disabled, as
```

## Source note 92, line 823

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L823)

```text
// dashboard features a recompiled title cannot reach.
```

## Source note 93, line 839

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L839)

```text
// Games has Achievements and Awards; Manage Game (downloadable content),
```

## Source note 94, line 840

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L840)

```text
// Title Updates and Active Downloads follow, plain entries made from
```

## Source note 95, line 841

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L841)

```text
// Awards.
```

## Source note 96, line 854

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L854)

```text
// Games & Apps gains Manage Game (downloadable content), below
```

## Source note 97, line 855

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L855)

```text
// Achievements, made from the Recent entry.
```

## Source note 98, line 857

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L857)

```text
// Title Updates below it, a page of its own: updates are not add-ons.
```

## Source note 99, line 862

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L862)

```text
// Settings gains Patches, Mods and Cheats below the console's settings
```

## Source note 100, line 863

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L863)

```text
// entry, made from the Preferences entry.
```

## Source note 101, line 910

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L910)

```text
// Achievements shows the gamerscore earned, beside the visual's own
```

## Source note 102, line 911

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L911)

```text
// gamerscore glyph (drawn as xui::kGamerscoreImage).
```

## Source note 103, line 968

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L968)

```text
// Player 1's pad, whose quadrant the ring lights. As on the console, wired
```

## Source note 104, line 969

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L969)

```text
// pads show no battery; so do wireless ones whose level the host cannot
```

## Source note 105, line 970

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L970)

```text
// read (a USB dongle that presents its pad as wired, for one).
```

## Source note 106, line 1040

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1040)

```text
/*exit_title=*/
```

## Source note 107, line 1067

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1067)

```text
// Keyboard: arrows, Enter or Space for A, Escape or Backspace for B, Y.
```

## Source note 108, line 1088

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1088)

```text
/*repeat=*/
```

## Source note 109, line 1204

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1204)

```text
// No wrap; the blade shuffle plays between neighbours.
```

## Source note 110, line 1221

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1221)

```text
/*initial=*/
```

## Source note 111, line 1237

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1237)

```text
// Turn Off Console is removed (kRemovedEntries).
```

## Source note 112, line 1238

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1238)

```text
// } else if (id == "btnShutdown") {
```

## Source note 113, line 1239

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1239)

```text
//   OpenConfirm(Confirm::kTurnOff);
```

## Source note 114, line 1267

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1267)

```text
// Unlocked first, then the rest in catalog order, as the console lists them.
```

## Source note 115, line 1292

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1292)

```text
// The scene ships its loading state: spinner shown, header shading hidden.
```

## Source note 116, line 1331

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1331)

```text
/*initial=*/
```

## Source note 117, line 1379

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1379)

```text
// FILETIME: 100 ns since 1601.
```

## Source note 118, line 1435

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1435)

```text
/*initial=*/
```

## Source note 119, line 1495

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1495)

```text
// The title is what was chosen: "Leave Game" (Y or the Home tab) or
```

## Source note 120, line 1496

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1496)

```text
// "Turn Off Console".
```

## Source note 121, line 1512

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1512)

```text
// Turning a title update on or off runs the other executable.
```

## Source note 122, line 1550

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1550)

```text
// Default to No, so a stray A does not end the session.
```

## Source note 123, line 1551

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1551)

```text
/*initial=*/
```

## Source note 124, line 1562

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1562)

```text
/*initial=*/
```

## Source note 125, line 1567

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1567)

```text
// the page's details and legends again
```

## Source note 126, line 1593

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_guide.cpp#L1593)

```text
/*exit_title=*/
```
