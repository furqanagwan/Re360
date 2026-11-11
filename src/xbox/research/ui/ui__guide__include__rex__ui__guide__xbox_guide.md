# Xbox guide: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/xbox_guide.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 69

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L69)

```text
/// Where the guide looks for the console's system update: the
```

## Source note 2, line 70

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L70)

```text
/// xbox_guide_system_update cvar, then `$SystemUpdate` beside the executable,
```

## Source note 3, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L71)

```text
/// then %LOCALAPPDATA%\ReXGlue\$SystemUpdate.
```

## Source note 4, line 74

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L74)

```text
/// The guide bundle the title build embedded (rexglue_configure_target), so
```

## Source note 5, line 75

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L75)

```text
/// players need nothing for the guide. Called by the generated registration
```

## Source note 6, line 76

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L76)

```text
/// at static initialisation; empty when the title was built without one.
```

## Source note 7, line 80

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L80)

```text
/// The scenes and strings the guide uses, parsed once from the owner's system
```

## Source note 8, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L81)

```text
/// update.
```

## Source note 9, line 82

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L82)

```text
/// The title host chooses presentation; asset availability never chooses it.
```

## Source note 10, line 83

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L83)

```text
/// OriginalXbox selects BC scenes, not an original Xbox execution backend.
```

## Source note 11, line 88

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L88)

```text
// huduiskin: control visuals, message boxes
```

## Source note 12, line 89

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L89)

```text
// xam hudbkgnd: the HUD frame the guide opens in
```

## Source note 13, line 90

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L90)

```text
// hud GuideMain: tabs and blades
```

## Source note 14, line 92

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L92)

```text
// The guide Microsoft's backward compatibility shows (GuideMainEmulator
```

## Source note 15, line 93

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L93)

```text
// and the *TabEmulator* scenes, in the PC backward-compatibility HUD): three
```

## Source note 16, line 94

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L94)

```text
// tabs, Games (1), Home (2) and Settings (3). Otherwise the 2.0.17559 guide
```

## Source note 17, line 95

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L95)

```text
// without Media: tabs 1, 2 and 4 (UseThreeTabs).
```

## Source note 18, line 97

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L97)

```text
// hud XboxOneXSettings (emulator layout only)
```

## Source note 19, line 101

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L101)

```text
// xam: the notification popup
```

## Source note 20, line 103

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L103)

```text
// Preferences and the pages built on its scenes (hud).
```

## Source note 21, line 106

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L106)

```text
// The on-screen keyboard (vk: KeyboardMain hosts KeyboardBase; RG-GDK-059).
```

## Source note 22, line 114

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L114)

```text
/// From a guide bundle (see EmbeddedGuide).
```

## Source note 23, line 123

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L123)

```text
/// The first string in `strings` starting with `prefix` (tables are per
```

## Source note 24, line 124

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L124)

```text
/// language; matching on English text keeps this independent of order).
```

## Source note 25, line 133

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L133)

```text
/// The guide's own vector drawings of the console's small images, for
```

## Source note 26, line 134

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L134)

```text
/// xui::RenderResources::vector_image: the legend button glyphs and the
```

## Source note 27, line 135

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L135)

```text
/// controller battery, sharp at any size.
```

## Source note 28, line 139

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L139)

```text
/// Hides the letters scenes lay over the button glyphs, which
```

## Source note 29, line 140

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L140)

```text
/// DrawGuideVectorImage draws itself.
```

## Source note 30, line 143

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L143)

```text
/// Adds the console's system font from the guide built into the title
```

## Source note 31, line 144

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L144)

```text
/// (RG-GDK-061), or Segoe UI, the host stand-in, without one. Call from the
```

## Source note 32, line 145

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L145)

```text
/// ImGui drawer's font setup, before the atlas is built. The glyphs are baked
```

## Source note 33, line 146

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L146)

```text
/// for `display_height` (the guide's text is sharp at 4K too).
```

## Source note 34, line 149

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L149)

```text
/// Textures and sounds from the system update, kept across openings.
```

## Source note 35, line 175

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L175)

```text
// the title's XDBF achievement icons
```

## Source note 36, line 178

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L178)

```text
/// The title's switchable code patches (null name ends the list).
```

## Source note 37, line 180

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L180)

```text
/// The title's own cheat codes (null name ends the list).
```

## Source note 38, line 182

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L182)

```text
/// The title's add-ons (null id ends the list), described by the built-in
```

## Source note 39, line 183

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L183)

```text
/// catalogue (EmbeddedDlcCatalog).
```

## Source note 40, line 185

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L185)

```text
/// The title update version the title was built with; 0 for none.
```

## Source note 41, line 187

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L187)

```text
/// The title's updates (zero version ends the list), for the Title Updates page, where
```

## Source note 42, line 188

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L188)

```text
/// the player can download one and turn it on: they are optional.
```

## Source note 43, line 190

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L190)

```text
/// The title's local data folder (%LOCALAPPDATA%\<name>): updates install
```

## Source note 44, line 191

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L191)

```text
/// under title_updates there.
```

## Source note 45, line 193

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L193)

```text
/// Restarts the title once it has closed, so a title update choice takes
```

## Source note 46, line 194

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L194)

```text
/// effect; the guide then ends the title as Leave Game does.
```

## Source note 47, line 196

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L196)

```text
/// The draw resolution scale that matches the display (3 for 4K).
```

## Source note 48, line 198

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L198)

```text
/// The Windows audio outputs the game can play through, for Audio Output
```

## Source note 49, line 199

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L199)

```text
/// (the audio_output_device setting); null hides the page.
```

## Source note 50, line 201

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L201)

```text
/// The displays the game can play on, in the monitor setting's order, for
```

## Source note 51, line 202

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L202)

```text
/// Display; null hides the page.
```

## Source note 52, line 204

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L204)

```text
/// Writes changed settings to the title's config file.
```

## Source note 53, line 206

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L206)

```text
/// Host copy/install jobs, newest first; read only on the UI thread.
```

## Source note 54, line 208

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L208)

```text
/// After the guide has closed; `exit_title` when the owner confirmed Xbox
```

## Source note 55, line 209

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L209)

```text
/// Home.
```

## Source note 56, line 213

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L213)

```text
/// The guide over a running title. It owns itself, as XAM dialogs do: it
```

## Source note 57, line 214

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L214)

```text
/// deletes itself after its close animation and then calls on_closed.
```

## Source note 58, line 221

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L221)

```text
/// Closes with the console's close animation (the chord pressed again).
```

## Source note 59, line 238

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L238)

```text
/// The tabs left to right: Games, Home and Settings (GuideAssets::emulator_layout).
```

## Source note 60, line 241

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L241)

```text
/// The next tab left (-1) or right (+1) of the current one.
```

## Source note 61, line 259

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L259)

```text
// Settings pages (guide_settings.cpp): Preferences and what it opens,
```

## Source note 62, line 260

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L260)

```text
// Patches and Cheats, each one of the console's own Options scenes.
```

## Source note 63, line 263

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L263)

```text
// focus on the page below
```

## Source note 64, line 265

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L265)

```text
// after focus moves on the page
```

## Source note 65, line 267

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L267)

```text
// X on the focused control
```

## Source note 66, line 280

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L280)

```text
/// Settings > Xbox Settings, emulator layout: the render resolution on the
```

## Source note 67, line 281

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L281)

```text
/// XboxOneXSettings scene's Graphics and Performance choice.
```

## Source note 68, line 287

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L287)

```text
// Games & Apps > Manage Game (guide_dlc.cpp): the title's add-ons from its
```

## Source note 69, line 288

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L288)

```text
// catalogue, installed from packages on this PC.
```

## Source note 70, line 290

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L290)

```text
// catalogue media ID; empty for content it lacks
```

## Source note 71, line 291

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L291)

```text
// a package on this PC for it, if found
```

## Source note 72, line 292

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L292)

```text
// installed content's file name
```

## Source note 73, line 294

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L294)

```text
// the display name its package carries
```

## Source note 74, line 300

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L300)

```text
// an install or file pick running off the UI thread
```

## Source note 75, line 307

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L307)

```text
// Home > Manage Storage (guide_storage.cpp): the title's saved games on
```

## Source note 76, line 308

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L308)

```text
// this PC, which the player can delete.
```

## Source note 77, line 310

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L310)

```text
// display name
```

## Source note 78, line 311

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L311)

```text
// content file name
```

## Source note 79, line 312

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L312)

```text
// 0 for saves common to every profile
```

## Source note 80, line 314

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L314)

```text
// last write, local time
```

## Source note 81, line 321

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L321)

```text
// Games & Apps > Title Updates (guide_title_update.cpp): the title's
```

## Source note 82, line 322

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L322)

```text
// updates, optional, downloaded and turned on or off. Not add-ons, so not
```

## Source note 83, line 323

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L323)

```text
// in Manage Game.
```

## Source note 84, line 327

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L327)

```text
/// The title update `row` is on the Title Updates page, or null.
```

## Source note 85, line 339

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L339)

```text
// Games & Apps > Active Downloads: title update downloads and installs.
```

## Source note 86, line 346

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L346)

```text
/// The Achievements row's gamerscore glyph in its label's colour: the skin
```

## Source note 87, line 347

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L347)

```text
/// draws it near white, unseen on an unfocused row.
```

## Source note 88, line 349

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L349)

```text
/// Shows player 1's battery, at most once a second.
```

## Source note 89, line 383

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L383)

```text
// kTitleUpdate: the version to run (0: the original)
```

## Source note 90, line 386

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L386)

```text
// detached once the backdrop stops
```

## Source note 91, line 397

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L397)

```text
// kDeleteSave: the index in saves_
```

## Source note 92, line 399

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L399)

```text
// a file pick's result
```

## Source note 93, line 400

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L400)

```text
// set when the pick is done
```

## Source note 94, line 409

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L409)

```text
// The right pane's banner: an XuiImage the Options scene does not have.
```

## Source note 95, line 415

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_guide.h#L415)

```text
// how far the title behind the guide is dimmed, 0 to 1
```
