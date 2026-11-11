# Xbox guide

The Xbox 360 guide over a running title, built from the console's own scenes
([ADR-011](adr/ADR-011-xbox-guide-from-system-xui.md),
[RG-GDK-041](https://github.com/furqanagwan/xbox/issues/1)).

Status: implemented in three parts: format layer, XUI runtime, guide. Checked
with Quantum of Solace (GDK Release, NVIDIA, 2026-09-30) through scripted
keyboard runs. An owner play session with a pad is still to do.

## Using it

- Open or close it with View and Menu together (Back and Start on an Xbox 360
  pad), as Xbox Series backward compatibility opens the 360 guide, or with Home
  (`bind_xbox_guide`). The Xbox button is deliberately not a trigger: on PC it
  opens Game Bar, and the guide leaves it to Windows.
- The guide is built into the title, so players need nothing for it and it
  works offline. Whoever builds the title sets `REXGLUE_SYSTEM_UPDATE` (a CMake
  cache variable, or the environment variable of that name) to their own
  console's `$SystemUpdate` folder once, as they supply the game itself.
  `rexglue_configure_target`, which every title calls, then runs
  `rexglue guide-bundle` to take the modules the guide reads (`hud`,
  `huduiskin`, `xam`, `gamerprofile` and the keyboard's `vk`) and embeds them
  in the executable with `.incbin`. The bundle is rebuilt when the update or
  the rexglue CLI changes, so a newer SDK takes what it needs. Nothing from the update ships with the SDK. Note that
  a title built this way carries those console files; anyone passing the
  executable on passes them on.
- Xbox 360 presentation is the default. The SDK target helper accepts
  `GUIDE_PRESENTATION xbox360` or `GUIDE_PRESENTATION original-xbox`.
  Xbox 360 builds use the console update even if `REXGLUE_GUIDE_FLASH` is set.
  Original Xbox presentation requires that Flash path, takes its BC modules
  ahead of supplementary console resources, and requires the emulator scenes.
  The full adapter accepts `GuidePresentation::Xbox360` (default) or
  `GuidePresentation::OriginalXbox` on `GuideAssets::Load`, `LoadBundle` and
  `FromUpdate`. It never automatically chooses presentation from available scenes.
  A BC presentation does not implement original Xbox execution or Xbox services.
  `rexglue guide-bundle <flash> <$SystemUpdate> -o <bundle>` still combines sources
  explicitly; the consuming host controls which scenes it loads.
- A build without `REXGLUE_SYSTEM_UPDATE` logs "Xbox guide not built in" and,
  at run time, falls back to the `xbox_guide_system_update` cvar, `$SystemUpdate`
  beside the executable, then `%LOCALAPPDATA%\ReXGlue\$SystemUpdate`. Naming
  the cvar also overrides the built-in guide. `--xbox_guide=false` turns the
  guide off.
- Navigation: D-pad or left stick; LB/RB or left/right switch tabs; A selects;
  B goes back or closes; Y is Leave Game; X is a page's extra action (Choose
  File for a title update). On the keyboard: arrows, Enter or Space, Escape or
  Backspace, X, Y, and Page Up/Down.

## What it does

It runs the console's own flow. The HUD backdrop plays `ClosedToHalf` and hosts
`GuideMain`, and the Home tab's blade comes in with `2Close`. Tabs change with
the `iToj` blade shuffles. Launching something plays `<tab>Open` with the
backdrop's `HalfToFull`. The Leave Game and Turn Off prompts are the skin's
`XuiMessageBox3`, hosted in the backdrop's error frame (`HalfToError`). Button
focus, press and sounds come from the skin visuals' named frames.

- Games & Apps > Achievements opens `802_Achievements` as a grid of the title's
  achievements. Unlocked ones show their XDBF icon, others the console's
  unearned or secret image. The header shows the focused achievement; A opens
  `828_AchievDetails`. The button shows the gamerscore earned. As on the
  17559 guide, this opens the current title's grid directly: `gp.xzp` has a
  games list only for Awards (`837_AvatarAwardGamesMe`), not for achievements.
- Leave Game (Y, or the Home tab's one entry, the console's Xbox Home renamed:
  a recompiled title has no dashboard) is worded as the Xbox One and Series
  consoles' guide words it for 360 titles. That guide also titles the Home
  tab with the gamertag, so the Home blade's labels show the profile's name
  (the gamertag of the Xbox account signed in to Windows; see below). Leave Game asks "Are you sure you want to close
  the game? Any unsaved progress will be lost." first, No focused. Yes closes
  the guide and ends the title through the window's normal close path.
- Home > Manage Storage (the backward-compatibility Home tab's second entry,
  `btnManageStorage`; dash command 75 on the console) lists this title's
  saved games on this PC, the profile's and any shared by every profile
  (`ContentManager::ListContent`, content type 1), each with its size, in the
  same Options scene as Manage Game. The right pane shows the size, when it
  was last saved and whose it is; A deletes it after the skin's message box
  asks ("It can't be recovered", No focused). A save the game has open is not
  deleted: the pane says it's in use.
- Games & Apps > Manage Game lists every add-on the title had in the
  marketplace (its config's `[[dlc]]` entries, described by the catalogue
  built into it; see [code patches](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/code-patches.md#title-add-ons)), as the
  console's in-game marketplace listed offers. Each row shows Install,
  Installed or Needs Update where the console showed the price; the right
  pane shows the add-on's banner, title, publisher, status and description.
  Install takes the add-on's package (STFS, content type 2, this title's ID,
  matched by display name) from the `DLC` folder beside the executable, or
  asks for it with the Windows file picker, and installs it with
  `ContentManager::InstallContent` in the background. An add-on whose
  `requires_title_update` is above the build's title update is not installed
  and says the title update is needed first. Installed or found content the
  catalogue lacks is listed after it. The page is the `OptionsNotifications`
  scene, its rows given the skin's `btn_Count` visual for the right-hand
  column, with an `XuiImage` added for the banner across the whole right
  pane at its top, edge to edge, as the console's marketplace showed it. The 17559 update has no
  in-game marketplace scenes (its dashboard's marketplace is Lua, in
  `contapp.xzp`), so the console's own offer list cannot be used. A game may
  need a restart to see new content. A description longer than the pane
  scrolls as the console's did: it holds at the top, scrolls slowly to the
  end, holds, fades out and fades back in at the top (`TextScrollAt`, for any
  wrapped text taller than its box), clipped to the pane.
- Games & Apps > Title Updates, below Manage Game and apart from the add-ons,
  lists the title's updates (its config's `[[title_update]]` entries) in the
  same Options scene, as Title Update rows showing Download, the
  download's progress, On or Off, or Not in Build when this build has no
  executable for it. They are optional, and the first run never asks for one.
  A downloads the update (Xbox Unity first, then any other sources, each
  failure listed in the pane) or, once it's installed, turns it on or off:
  the skin's message box asks first, since the game restarts into the other
  executable, then the guide ends the title and it starts again. X chooses
  the update's package from this PC instead. The right pane shows the
  status, release date, size, changelog, and that mods made for the original
  version are left out while it's on
  ([title updates](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/title-updates.md)).
- Games & Apps > Active Downloads lists this session's title update
  downloads and installs, newest first, with their progress or result; A on a
  running download cancels it. Downloads carry on when the guide is closed.
- A tab whose entries no longer fit its scene (Games & Apps has nine with the
  added entries; its scene is 200 units, seven 28-unit rows) is clipped to the
  scene and scrolls a row at a time to keep the focused entry in view.
- Settings > Preferences, Patches, Mods and Cheats open settings pages built
  from the console's own Options scenes (see [Settings pages](#settings-pages)).
- While the guide is open, the title behind it is darkened to a quarter of its
  brightness, fading in and out with the HUD's ClosedToFull and FullToClosed
  animations. XAM does this in code (`hudbkgnd` has no such element); the
  amount is measured from a capture of the 17559 guide over a title.
- Beside the clock, player 1's controller battery and the ring of light with
  player 1's quadrant lit green, as XAM sets them. The battery comes from
  `InputSystem::GetBattery`, read at most once a second. GuideMain names
  `imgControllerBattery`'s frames Little, Low, Medium and High (0 to 3); the
  guide shows frame 0 below 15%, 1 below 45%, 2 below 75% and 3 above. As on
  the console, a wired pad shows no battery. Neither does a wireless pad
  whose level the host cannot read. See
  [GameInput: Bluetooth pads and battery](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/gameinput.md#bluetooth-pads-and-battery-rg-gdk-047).
- Entries a recompiled title has no use for are taken out and the list closed
  up: Home > Connect to Xbox Live and the disabled title entry (Disc in Tray),
  Settings > Family Settings, Account Management, Kinect Tuner and Turn Off
  Console, and the whole Media tab. The guide has three tabs, Games & Apps,
  Home and Settings, as on the Xbox One and Series consoles' guide for 360
  titles. GuideMain's timelines are rewritten for them when the scenes load
  (`UseThreeTabs`, `guide_layout.cpp`): every switch moves each blade one
  slot, so a side with one tab fewer is the same motion with its outermost
  blade hidden (Blade5 on the right for Games & Apps and Home, Blade6 on the
  left for Settings) and each label on its inner neighbour's track. Home to
  Settings plays the scene's Media-to-Settings frames (`3To4`, renamed
  `2To4`), with Home's content and selected label fading as Media's did: one
  shuffle, no pass over a removed tab. Settings > System Settings is renamed
  Xbox Settings (the Series consoles' Xbox One X Settings) and stays disabled.
  The code is kept and commented or listed (`kRemovedEntries`, `kRemovedTab`
  in `xbox_guide.cpp`), so each can be put back.
- With `REXGLUE_GUIDE_FLASH`, the guide is the one Microsoft's PC backward
  compatibility shows, from the BC `hud.xex`'s own emulator scenes:
  `GuideMainEmulator` has three tabs (Games, Home and Settings: `Tab1` to
  `Tab3`, their own `1To2` to `3To2` shuffles) and a Y legend of Leave Game, so
  nothing is rewritten. Home is `HomeTabEmulatorSignedInLocal` (Leave Game and
  Manage Storage, for a profile without Xbox Live), Games
  `GamesTabEmulatorSignedIn` (Achievements, Awards) and Settings
  `SettingsTabEmulatorSignedIn` (Profile, Preferences, Xbox One X Settings).
  Preferences is `OptionsEmulator`. Manage Game, Title Updates and Active
  Downloads are added below Awards, as `XuiButtonGuide` rows (`AddEntry`'s
  `visual`), and Patches, Mods and Cheats below Xbox Settings. Xbox One X
  Settings is renamed Xbox Settings and opens its own scene,
  `XboxOneXSettings` (see [Settings pages](#settings-pages)). Manage Storage
  opens the title's saves (see below). Without the emulator scenes (a 17559-only build) the guide
  is the 17559 one above.
- Everything else (Marketplace, My Games, media players, Live features) stays in
  the menu, disabled, as the console's disabled controls behave: they take focus,
  and pressing them plays the inactive sound.
- While it is open, the guest sees a neutral pad, XN_SYS_UI is true, and
  XamIsUIActive reports system UI, as for the Guide button on the console.
  Titles that pause for XN_SYS_UI pause.

On the console the Guide button never reaches the title. View+Menu does, so a
title may react to the Menu (Start) press that completes the chord, for example by
opening its pause menu. The guide masks the buttons once it is open.

## Achievement notifications

Unlocks play XAM's own popup, `xam/xam notify.xur` with the skin's
`scr_Notification` visual. The Xbox sphere bursts in, the ring of light flashes
green, the bar slides out with the trophy, and `NotifyPopup.xma` plays. The text
is XAM's "Achievement unlocked\n%sG - %s". The popup sits at the bottom centre of
the HUD space, one unlock at a time. The popup's TransTo ends in a go-to that can
hold it, so it leaves with TransFrom after 4 s, which is the console's timing as
remembered, not measured. Without a system update, the SDK's own toast shows the
unlock. The console command `achievement_notify [id]` shows an achievement's
popup without unlocking it.

## On-screen keyboard (RG-GDK-059)

`XamShowKeyboardUI` shows the console's own keyboard from the same built-in
files. The guide bundle now carries `vk` (`$flash_vk.xex`, the keyboard), whose
`vk/vk` package holds `KeyboardMain` (a HUD scene with the title) and
`KeyboardBase`:

- the description;
- the text field (`scr_Edit`, with its own caret, placed after the characters
  before the cursor and blinking);
- a 5 x 10 grid of keys, with Backspace and Space below;
- a column of three keys on each side: Left, the previous page and Caps;
  Right, the next page and Done.

It opens like XAM's other full-screen UI, in the HUD backdrop with
`ClosedToFull`, over the darkened title, and closes with `FullToClosed`.

- **Pages.** The English pages of the 2.0.17559 keyboard, from `vk.xex`'s own
  tables (`VirtualKeyboard`):
  - Alphabet (QWERTY): `1234567890`, `qwertyuiop`, `asdfghjkl-`, `zxcvbnm_@.`,
    and a blank fifth row;
  - Symbols;
  - Accents.

  Caps gives capitals. The side keys show the console's pictures (LB, RB, LT,
  RT, the left stick, Start) and their page names.
- **Pad**, as on the console:
  - A presses the focused key and B cancels;
  - X is Backspace and Y is Space;
  - LB and RB move the cursor;
  - LT and RT change the page;
  - the left stick pressed in is Caps;
  - Start is Done.

  The D-pad or the stick moves over the keys and wraps.
- **PC keyboard.** Typing goes straight into the field. Backspace deletes,
  Enter is Done, Escape cancels and the arrows move over the keys. Keys held as
  the keyboard opens are ignored until released.
- **Result.** The text goes to the title's buffer (cut to its length);
  cancelling gives `X_ERROR_CANCELLED`. Without the keyboard scenes (no update
  built in) the SDK's ImGui dialog is shown as before.
- **Not yet.** Only the English pages and the full keyboard are done. The flag
  modes (email, numeric, password and others, `flags`, logged on each call)
  are not applied: their values are not recorded yet. The other languages'
  pages (Russian, Polish, Greek, Czech, Turkish, Japanese, Korean, Chinese) are
  in `vk.xex` and not yet read.
- **Tools.**
  - The console command `keyboard_test [default text]` shows the keyboard a
    title gets and logs the result.
  - `rexglue xui-dump <sources> -o <dir>` writes out every file of the built-in
    modules' packages, for research.

## Settings pages

Each page is one of the dashboard's Options scenes, hosted like Achievements
(`HalfToFull`, the blade goes out; B comes back with `FullToHalf`). Unused
controls are hidden and the rest moved up; new entries are copies of the
scene's own controls (`guide_layout.h`: `RemoveEntry`, `AddEntry`). Every change
is saved to the title's config file straight away.

| Page | Scene | Controls | Setting |
| --- | --- | --- | --- |
| Preferences | `Options` (`OptionsEmulator` with the BC HUD) | Notifications, Volume (the Voice entry), Audio Output and Display (added below Volume when the host lists outputs and displays), Vibration, Resolution (a copy of Vibration; with the BC HUD it is under Xbox Settings instead). Online Status, Family Timer and Word Registration are removed. | |
| Xbox Settings (BC HUD) | `XboxOneXSettings` | Optimize game for: Graphics or Performance. The pane's first line, "Changing this setting will end your current session.", reads that the setting is used at the next start. | Graphics: `resolution_match_display`; Performance: `resolution_scale` 1; next launch |
| Notifications | `OptionsNotifications` | Show Notifications; Play Sound (disabled while Show is off) | `notifications_show`, `notifications_sound`: the unlock popup and its sound |
| Volume | `OptionsVoice` | Game Volume slider, steps of 10, left and right; the Kinect checkbox as Mute When Minimized; voice and output hidden | `audio_volume`, `audio_mute_minimized`, applied live |
| Audio Output | `OptionsVoice`'s Play Through radio list, rows added past three | Windows Default, then each active Windows output (up to seven) by its device name, the adapter in brackets dropped unless two share a name; the panel shows the focused output's full name, its speakers and format, whether the game plays in 5.1 there or is folded to stereo, spatial sound, and which of Dolby Digital, Dolby Digital Plus, Dolby TrueHD (Atmos), DTS and DTS-HD it decodes. A chosen output that is unplugged shows as Last Chosen (Not Connected) and the game plays through the default meanwhile | `audio_output_device` (endpoint ID, empty for the default), applied live: the game's sound and the guide's own sounds move at once |
| Display | `OptionsVoice`'s Play Through radio list, headed Play On | Each display by its monitor name, in the `monitor` setting's order; the panel shows whether it is the main display and has the game, its current mode, the fastest refresh of its native and standard resolutions, HDR (supported, on or off in Windows), peak brightness, bits per colour, and that the game draws in SDR | `monitor`, applied live: the game moves to the display |
| Vibration | `OptionsController` | Enable Vibration | `vibration`, applied live |
| Resolution | `OptionsVoice`'s output radio list, one button added | Original (the default), 2x, 3x and Match Display, the last three marked Experimental | `resolution_scale`, `resolution_match_display`; next launch |
| Patches, Mods | `OptionsNotifications` checkboxes, one copy per patch | The title's switchable code patches of that category (`patch`, `mod`) | `code_patch_states`, applied live |
| Cheats | `OptionsNotifications` checkbox copies without the box, one per code | The title's own cheat codes (`[[cheat]]`): the panel shows the code, what it unlocks and where the game takes it | none; nothing is patched |

- **Resolution.** By default the title draws at its own resolution and is
  scaled to the screen, as the console does (most titles drew at 720p or
  less). 2x and 3x multiply that; they are marked Experimental because they
  cost far more GPU time: Quantum of Solace's gameplay lost frames at 3x on a
  4K display (RG-GDK-046), where the same save held 55 to 60 fps at 1x. Match
  Display (`resolution_match_display`, now off by default) sets
  `resolution_scale` at startup from the window's monitor (2160p gives 3,
  1440p and 1080p give 2) as a one-run value, so a config file's
  `resolution_scale` is kept, and `--resolution_scale` on the command line
  still wins. The draw scale needs a restart, so a choice made in the guide
  applies at the next launch; the page says so.
- **Controller battery.** The console's battery icons are 38 x 14 PNGs drawn
  through `Controller_*.xur` scenes. The guide redraws them as shapes traced
  from those PNGs (`RenderResources::vector_image`), so they are sharp at 4K.
- **Patches and Mods** list the title's
  [switchable code patches](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/code-patches.md#switchable-patches) by their
  `category`: fixes such as the frame rate under Patches, changes to play
  (a trainer's infinite ammo) under Mods. Turning one on or off takes effect
  at once and is kept in `code_patch_states`. A title with none shows "No
  mods are available for this game."
- **Cheats** lists the cheats the title's developers built in, the codes a
  player types into the game's own menu, from the title's
  [`[[cheat]]` entries](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/code-patches.md#title-cheat-codes). It is a reference:
  the game still takes the code. A title with none shows "This game has no
  cheat codes."
- **4K.** The guide's figures, gradients and text are drawn at the display's
  resolution; the fonts are baked for the display's height (120 px at 2160p),
  so text and notifications are sharp at 4K. The console's images (PNG) and the
  title's achievement icons (64 x 64) have no higher-resolution source and are
  scaled up with linear filtering.
  - The host lays ImGui out in logical units (1280 x 720 at 4K with 300%
    Windows scaling) and the draw lists are stretched to the physical pixels.
    The renderer is told the pixels per unit (`RenderResources::pixels_per_point`,
    the window's DPI over 96) and slices radial gradients for the physical
    pixels.
  - The skin draws its discs (the radio buttons' ring, fill and dot) as radial
    gradients that fade to transparent over their last 20% (14% for the dot):
    about 1.5 pixels of soft edge at 720p. Scaled to 4K that fade became a
    5-pixel blur. A radial gradient whose last stop fades its colour out keeps
    the fade's width on screen, around the same middle (`EdgeFadeOnScreen`), so
    the discs are as crisp as on the console. Measured on the Resolution page
    at 3840 x 2160 and 300%: the ring's edges went from 6-pixel ramps to 1–2
    pixels.
  - The button letters (A, B, X, Y) are drawn as one glyph quad at the exact
    centre of their disc. `ImDrawList::AddText` truncates its position to whole
    display units, three pixels each at 300%, which left the B 2.5 pixels left
    and both letters 2.5 pixels high.
  - The tab label column (`Blade_Focus`, from y 135) starts a unit inside the
    centre blade's visible top and left edges. Under a pixel at 720p, at 4K
    the blade showed through as a 2-pixel white line above and beside the
    column; the column now starts a unit up and left (`CoverBladeEdge`; the
    360 HUD only).

## Menu inventory

All entries are from the 17559 scenes, in the order they appear. "Works" means
the guide acts on it; everything else is shown disabled, as on the console.

| Tab | Entry | Opens on the console | Here |
| --- | --- | --- | --- |
| Games & Apps | Achievements | `802_Achievements` grid, then `828_AchievDetails` | Works |
| | Manage Game (added) | | Works: the title's add-ons from its catalogue, installed from this PC |
| | Title Updates (added) | | Works: the title's updates, optional, downloaded and turned on or off |
| | Game Update (added) | | Works: new releases of the recompiled game, downloaded, checked and installed by its updater |
| | Awards | `837_AvatarAwards` (avatar awards) | Disabled |
| | Recent | `QuickLaunch`: Games & Apps, Downloads, All tabs | Disabled |
| | My Games | dashboard (dash command 23) | Disabled |
| | Active Downloads | download queue | Works: title update downloads and installs |
| | Redeem Code | code entry | Disabled |
| | Activity Feed (hidden in 17559) | | Hidden |
| Home | Xbox Home | quit prompt, then the dashboard | Renamed Leave Game: prompt, then ends the title; the tab is titled with the gamertag |
| | Connect to Xbox Live (offline) or Friends, Party, Messages, Beacons & Activity, Chat (on Live) | Live features | Removed |
| | Disc in Tray | title name; ejects | Removed |
| Media | System Video Player, System Music Player, Picture Viewer, Windows Media Center | dashboard apps (dash 39, 6, 44, 8); mini player below | Removed (the tab) |
| Settings | Profile | gamer profile | Disabled |
| | Preferences | `Options`: Word Registration, Family Timer (`OptionsPlayTimer`, Add More Time), Vibration (`OptionsController`), Voice (`OptionsVoice`: volumes, output), Notifications (`OptionsNotifications`), Online Status (`OptionsOnline`) | Works: [settings pages](#settings-pages) |
| | System Settings | dashboard (dash 47) | Renamed Xbox Settings; disabled |
| | Patches, Mods (added) | | Works: switchable code patches |
| | Cheats (added) | | Works: the title's own cheat codes |
| | Family Settings, Account Management | dashboard (dash 20, 10) | Removed |
| | Kinect Tuner | Kinect troubleshooter | Removed |
| | Turn Off Console | turn-off prompt | Removed |
| Y button | Xbox Home | as above | Leave Game, as above |

## Where the guide comes from

Dashboard 2.0.17559's system update (`$SystemUpdate`, as a USB update or console
dump) holds the package `su20076000_00000000` (PIRS/STFS). Its `$flash_*.xex`
files are unencrypted, LZX-compressed XEX2 images whose resources are XUIZ
packages:

| Package | Contents the guide uses |
| --- | --- |
| `hud/hud` | `GuideMain.xur`; the tab scenes `HomeTab*`, `GamesTab*`, `SettingsTab*`; `InfoMessage.xur`; `Strings.xus`; `BladeOpen.xma`, `BladeSwitch_1..4.xma`; battery and media icons |
| `huduiskin/skin` | `skin.xur`: the visuals controls name (`XuiButtonGuide`, `btn_Count_achiev`, `HUD_Bladedark`, `HUD_Bladegrey`, `legend_A/B/X/Y`, `Label_Head`, `XuiMessageBox2/3/4`...) |
| `huduiskin/xam` | `XamStrings.xus` (the Xbox Home confirmation, the achievement toast text) |
| `xam/xam` | `hudbkgnd.xur` (the backdrop's state machine), `HUD_open.xma`, `HUD_close.xma`, `Achievement.png` |
| `xam/skin` | blade nine-grid images, `btn_selectG.xma`, `btn_backG.xma` |
| `xam/shrdres` | button glyphs (`A-Button.png`...), `btn_Focus.xma`, `btn_Select.xma`, `btn_Back.xma`, `tab_Switch.xma`, achievement icons |
| `gamerprofile/gp` | `802_Achievements.xur`, `828_AchievDetails.xur` |

Scene paths are either relative to the scene's own package or use
`sharedres://`, which means `xam/shrdres`. Skin visuals name images that live
in `xam/skin`.

## XUR v8

All values are big-endian. A packed integer is one byte below `0xF0`, `0xFnnn`
in two bytes, or `0xFF` followed by 32 bits.

- Header: `XUIB`, version 8, flags, tool version (u16), file size, section
  count (u16). Then a count header of 12 packed totals, where the first is the
  object count. Then the section table: magic, offset and length per section.
- Pools: `STRN` (u32 length, u16 count, NUL-terminated UTF-8; index 0 is the
  empty string), `VECT`, `QUAT`, `FLOT`, `COLR` (ARGB), `CUST` (figure paths:
  length, box, point count, then anchor and two control points per point).
- `KEYP` holds packed keyframe values: literals for bool and integer types,
  pool indexes for the others. `KEYD` holds keyframes: packed frame, then a flag
  byte (0 linear, 1 none, 2 ease followed by signed ease-in, ease-out and scale
  bytes; 3, 0xA and 0xB carry extra data the runtime ignores), then the index of
  the first `KEYP` value. `NAME` holds named frames: name, frame, command (play,
  stop, go to, go to and play, go to and stop), and a target for the go-to
  commands.
- `DATA` is the element tree. Per element: the class name's string index, then
  a flag byte: 1 own properties, 8 shares an earlier element's properties
  (packed index), 2 has children, 4 has named frames and timelines. Properties
  start with a packed value count, then one packed mask per class, base class
  first, followed by that class's set values. Compound values (Fill, Gradient,
  Stroke) are written once and referenced by index after that. Indexed
  properties (gradient stops) carry a byte count. Timelines animate the owner's
  descendants. Each names its target by Id, then its properties as a class index
  into the target's chain (derived class first) and a property index per compound
  level, then its keyframe count and first `KEYD` index. A leaf element with
  flag 4 has no timeline count.

The schema (`ui/xui/src/schema.cpp`) lists only the classes the guide's scenes
use. Mask bits past a class's known properties are read as one packed value and
ignored, since every non-compound type except bool is one packed value (bool is
a byte, which reads the same). One finding: `AccountManagementNavButton` derives
from `GuideDashCommandNavButton`, which the Settings tab relies on.

## Inferred, not documented

These are unknowns kept deliberately. Revisit them if the runtime looks wrong
against the console.

- Timeline rate: 60 frames/s. A blade switch is 11 frames, about 183 ms. The
  reference recreation times it at about 155 ms. 30 frames/s would be twice
  as slow as the console feels.
- Ease: bytes are signed percentages. The guide's blades use in -100, out 100.
  The runtime's curve is a cubic Bezier fit, not XUI's own formula.
- Text style bits come from the skin's named label visuals (`...Right`,
  `...Center`, `...Ellipsis`, `...NoWrap`, `..._V`): 0x10 no wrap, 0x200 right,
  0x400 centre, 0x1000 vertical centre, 0x4000 ellipsis, 0x1 bold (button
  labels and legend letters have it, the header does not). 0x4, 0x100,
  0x4000000 and 0x8000000 are unknown and ignored. Message box bodies
  (`XuiEdit`) wrap.
- Anchor bits: 1 left, 2 top, 4 right, 8 bottom, 0x10/0x20 centre, 0x40/0x80
  scale. They fit how the button visuals stretch.
- Radial fills: the brush is the box's inscribed ellipse, moved against the
  fill's Translation (box units, turned by the fill Rotation) and sized by its
  Scale. That is what puts the notification's ring-of-light arcs round the logo
  and the bar's rounded end in place; the opposite sign draws them outside.
  3D rotations are drawn flat (a half turn about X or Y mirrors).
- A Fill without FillType is solid; a Stroke without StrokeWidth draws
  nothing. The separators and focus tabs only look right that way.
- GuideMain's `<tab>Open`/`<tab>Close` frames take a tab's blade out and bring
  it in. The names read backwards until the keyframes are checked (Tab2's
  opacity falls in `2Open`).
- Fonts (RG-GDK-061): the console's `.xtt` fonts are not encrypted but
  repacked. After a 0x118-byte header (magic `xttf`, a 256-byte signature,
  then signed, file, compressed and uncompressed sizes and a version) comes a
  zlib-compressed sfnt directory with the standard tables plus Xbox ones:
  `xglf` (at its offset in the file) holds the glyph outlines as separately
  zlib-compressed 4 KiB blocks, `xloc` locates glyph *n* as
  `(block << 16) | offset` within the inflated block, and `xchk` is a SHA-1
  per `xglf` block. `xui::XttToTrueType` rebuilds glyf and loca from them,
  keeps the font's own tables and builds the maxp, OS/2 and post a font lacks
  (`xenonjklatin.xtt`, 20,334 glyphs, has none; `SegoeXbox-Light.xtt` has
  all three). The bundle keeps the `.xtt` files as they are, keyed
  `font/<name>`, and the guide draws its text in `xenonjklatin` ("Xbox JK")
  when the bundle has it; without it, Segoe UI stands in. The atlas is built
  once (ImGui's legacy mode), so only glyphs in the font's ranges exist: Latin
  with Extended-A and -B, Greek, Cyrillic, punctuation, euro and trade mark,
  plus every character of the guide's string tables. The 17559 update's
  `$flash_xenonjklatin.xttp` (28 KB) and `$flash_xenonclatin.xttp` (96 KB) are
  patches XAM applies to the console's flash fonts, which no build has; the
  backward-compatibility `xenonjklatin.xtt` the bundle takes is a whole font,
  so the patches are not used. Its private-use gamerscore glyph (U+E00A, in `btn_Count_achiev`
  and in the points XAM puts beside achievements) is drawn as an image: the
  shape of sharedres `GScore_white.png` (a white disc with a G cut out, so the
  row's colour shows through) traced at 256 pixels, tinted with the text
  colour (`xui::kGamerscoreGlyph`, `kGamerscoreImage`). The skin draws the
  Achievements row's glyph near white whatever the row's state, unseen on an
  unfocused row, so the guide gives it the row's label colour.
- Row separators: the skin's `Top` and `Bottom` figures (0xffd2d5d9) have
  BlendMode 1, multiply, so each darkens the row under it by the same
  proportion and every line looks equally dark. The renderer draws a
  multiplied grey g as black at alpha 1 - g; drawn as a plain light grey they
  faded into the darker rows lower down.
- Button glyphs: sharedres `A-Button.png` to `Y-Button.png` are 18-pixel discs
  with the letter laid over by a separate text element placed for the
  console's font, so they were soft at 4K and their letters off centre. The
  guide draws each as a disc in the PNG's colour with its letter, bold, the
  centre of its ink on the disc's centre (`DrawButtonGlyph`), and hides the
  scenes' letter elements.

## Gamertag

The profile (what titles get from `XUserGetName` and what the Home tab
shows) is named after the Xbox account signed in to this PC: the Xbox app's
sign-in keeps its gamertag in `HKCU\Software\Microsoft\XboxLive`
(`Gamertag`, the classic form, cut to the 360's 15 characters). That key is
not a documented interface; the GDK's `XUserGetGamertag` would be, but
`XUserAddAsync` fails with `E_GAMEUSER_NO_PACKAGE_IDENTITY` (0x89245110) for a
game without a Store package identity and a title registered with Microsoft,
which a recompiled title is not (checked 2026-10-01, GDK 260404). The
`user_gamertag` cvar overrides it; with neither, the name is `User`. The
profile's XUID is unchanged, so save locations do not move.

## Validation

- 007 Legends, 2026-10-01, 3840 x 2160 (RG-GDK-051): three blades on Home
  (Games & Apps, User, Settings), two on the left of Settings and two on the
  right of Games & Apps, none hidden behind the panel; Home to Settings and
  back play one shuffle each. A, B and Y are sharp discs with centred
  letters. The Achievements row's G shows with Manage Game focused. Manage
  Game's banner spans the right pane. The three-tab timelines are also
  checked against the 17559 scene by a local unit test
  (`REXGLUE_SYSTEM_UPDATE`).
- Quantum of Solace and 007 Legends, 2026-10-01, 3840 x 2160, against two
  photos of the 17559 guide (Xbox Guide over a title, and Console Settings):
  text was a quarter too small (XUI point size to scene units 1.2, now 1.6;
  "Xbox Home" now stands 24 of its row's 61 pixels at 1080p, as on the
  console); the title behind is darkened; the clock's battery and ring of
  light appear, player 1 green. Panel, blade, highlight and text colours
  already matched once the photo's limited range (black at 16) is allowed
  for. The ring's quarter arcs had curved the wrong way: a radial brush's
  Scale is a texture scale, so 0.55 makes the ellipse larger, not smaller.
  Legends' Settings > Mods listed its six trainer mods and Settings > Cheats
  its eight codes, each with its code and where to enter it.

- `unit_tests [guide]`: the chord (once per press, held-at-start, Guide
  button), pad actions (buttons held at open ignored, direction repeat, stick,
  bumpers).
- `unit_tests [xui]` also covers the runtime: visuals and anchoring, frame 0 on
  build, eased playback and stop frames, sound cues, per-element compound
  animation, navigation over hidden controls, focus, path resolution; and
  `[ui_sound]` covers XMA file decoding. With `REXGLUE_SYSTEM_UPDATE`, `[local]` also
  plays GuideMain's `2To3` shuffle and decodes every guide sound (all audible).
- Quantum of Solace, 2026-09-30, 3840 x 2160 display: drew at 2160p
  (`resolution_scale` 3 from the display); Settings tab reached over the removed
  Media tab; Preferences, Notifications (both toggles, Play Sound disabled with
  Show off), Volume (100 to 70, saved), Vibration, Resolution (1280 x 720
  chosen, saved); Patches listed "Unlock FPS" on and turned it off; Cheats
  showed none. Settings were saved to a throwaway config beside the exe.
- `unit_tests [xui]`, `[guide]`: `Seek`, `RemoveEntry`/`AddEntry` (list closed
  up, navigation relinked), saved patch states by name.
- Quantum of Solace, 2026-09-30: opened with Home; Home tab, Games & Apps,
  Media; Achievements grid (50, 0 unlocked) and details; Xbox Home prompt with
  cancel and reopen through Y; close. No errors in the log.
- `unit_tests [xui]`: synthetic XUIZ, XUIS, XUR v8 (elements, shared and
  compound properties, gradient stops, timelines with compound paths, named
  frames, truncation, unknown classes, object count) and XEX2 resources (plain
  and basic compression, encrypted images refused).
- `unit_tests [keyboard]`:
  - the console's English pages and capitals;
  - editing at the cursor within the buffer's length;
  - Start, the left stick and the triggers from the pad.

  Quantum of Solace, 2026-10-02: `keyboard_test 007`, then typing "bond" and
  Enter, returned "007bond"; Escape returned cancelled. The keyboard drew with
  the console's scenes and pictures at 3840 x 2160.
- `unit_tests [xtt]`: a synthetic two-block XTT converts (loca, maxp,
  checksums), damaged ones are refused and fonts round-trip through the
  bundle. With `REXGLUE_GUIDE_FLASH` and/or `REXGLUE_SYSTEM_UPDATE` set, every
  console font found converts (2026-10-02: `xenonjklatin` from Fuzion Frenzy's
  Flash, `SegoeXbox-Light` and `XenonSCLatin` from 17559).
- `unit_tests [guide][local]` with `REXGLUE_GUIDE_FLASH`: the guide assets take
  the emulator layout (Leave Game, Home without Connect to Xbox Live, Xbox
  Settings and Preferences scenes found) and `GuideMainEmulator` plays `2Close`,
  `2To3`, `3To2` and `2To1` with each tab shown in turn; `AddEntry` with a
  `visual` gives the copy that visual.
- Quantum of Solace, 2026-10-02, built with Fuzion Frenzy's Flash and 17559:
  the emulator guide's Home (Leave Game, Manage Storage disabled), Games
  (Achievements with gamerscore, Awards disabled, Manage Game, Title Updates,
  Active Downloads), Settings (Profile disabled, Preferences, Xbox Settings,
  Patches, Mods, Cheats), Xbox Settings (Performance checked) and Preferences
  (Notifications, Volume, Vibration), at 1280 x 720, 1920 x 1080, 2560 x 1440
  and 3840 x 2160. 7680 x 4320 was not tested: the only display is 4K and
  Windows keeps a window within it. No errors in the log.
- `unit_tests [local]` with `REXGLUE_SYSTEM_UPDATE` set to a `$SystemUpdate`
  folder: loads the package through the SDK's STFS device and LZX decoder, then
  decodes every non-Kinect scene in `hud/hud`, `huduiskin/skin`, `xam/xam` and
  `gamerprofile/gp` (2026-09-30: all pass on the 17559 package recorded in the
  [tracking ledger](https://github.com/furqanagwan/rexglue-sdk/blob/d1a87b4ef0a09c7a7813ab2a2b27976de01de203/docs/upstream-tracking.md)). The Kinect scenes use ControlPack
  classes outside the schema.
