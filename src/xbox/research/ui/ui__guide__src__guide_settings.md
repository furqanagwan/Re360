# Guide settings: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/guide_settings.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 41

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L41)

```text
// A slider's body shows its value at frames 0..100, and focused at
```

## Source note 2, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L42)

```text
// 101..201 (the skin's XuiSlider visual).
```

## Source note 3, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L72)

```text
// Windows names an output "<device> (<adapter>)"; the rows show the device,
```

## Source note 4, line 73

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L73)

```text
// unless two outputs share it.
```

## Source note 5, line 95

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L95)

```text
// The Voice scene's Play Through radio list (headset, speakers or both) with
```

## Source note 6, line 96

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L96)

```text
// a row per text, rows past its three copied from its last, below `heading`.
```

## Source note 7, line 97

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L97)

```text
// Empty when the scene has no such list.
```

## Source note 8, line 136

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L136)

```text
// Radio buttons: one checked in the group.
```

## Source note 9, line 149

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L149)

```text
// As for Achievements: the HUD goes full height, the blade goes out.
```

## Source note 10, line 191

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L191)

```text
/*initial=*/
```

## Source note 11, line 199

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L199)

```text
// Copies: a handler may push a page, which moves pages_.
```

## Source note 12, line 246

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L246)

```text
// Online Status, Family Timer and Word Registration are Xbox Live and
```

## Source note 13, line 247

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L247)

```text
// console features; a recompiled title has none of them.
```

## Source note 14, line 259

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L259)

```text
// The render resolution is under Xbox Settings where the guide has it.
```

## Source note 15, line 279

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L279)

```text
// The scene lists its entries bottom up; focus starts at the top one.
```

## Source note 16, line 281

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L281)

```text
/*initial=*/
```

## Source note 17, line 299

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L299)

```text
/*initial=*/
```

## Source note 18, line 312

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L312)

```text
// Voice Volume and voice output are for chat; Game Volume is the title's
```

## Source note 19, line 313

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L313)

```text
// output level (audio_volume). The Kinect checkbox, which the console shows
```

## Source note 20, line 314

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L314)

```text
// only with a Kinect, becomes Mute When Minimized (audio_mute_minimized),
```

## Source note 21, line 315

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L315)

```text
// as Microsoft's PC backward compatibility silences a minimised title.
```

## Source note 22, line 361

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L361)

```text
/*initial=*/
```

## Source note 23, line 367

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L367)

```text
// Notifications here are achievement unlocks; videos, TV and the
```

## Source note 24, line 368

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L368)

```text
// console's sound setting do not apply.
```

## Source note 25, line 377

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L377)

```text
// Play Sound belongs to Show Notifications, as on the console.
```

## Source note 26, line 391

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L391)

```text
/*initial=*/
```

## Source note 27, line 396

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L396)

```text
// The Voice scene's output choice is the console's radio list; its
```

## Source note 28, line 397

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L397)

```text
// sliders and Kinect option are not used.
```

## Source note 29, line 408

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L408)

```text
// 0: match the display
```

## Source note 30, line 459

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L459)

```text
/*initial=*/
```

## Source note 31, line 539

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L539)

```text
/*initial=*/
```

## Source note 32, line 588

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L588)

```text
/*initial=*/
```

## Source note 33, line 595

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L595)

```text
// Optimize game for: Graphics draws the game at the display's resolution
```

## Source note 34, line 596

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L596)

```text
// (a multiple of the console's), Performance at the console's own. A
```

## Source note 35, line 597

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L597)

```text
// resolution takes effect at the next start, not by ending the session.
```

## Source note 36, line 627

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L627)

```text
/*initial=*/
```

## Source note 37, line 640

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L640)

```text
// One checkbox per patch, copies of Show Notifications, in its place.
```

## Source note 38, line 682

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L682)

```text
/*initial=*/
```

## Source note 39, line 686

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L686)

```text
// The title's own cheat codes: nothing to switch, so each row is the
```

## Source note 40, line 687

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L687)

```text
// checkbox copy without its box, and the panel says what the code does
```

## Source note 41, line 688

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L688)

```text
// and where the game takes it.
```

## Source note 42, line 733

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_settings.cpp#L733)

```text
/*initial=*/
```
