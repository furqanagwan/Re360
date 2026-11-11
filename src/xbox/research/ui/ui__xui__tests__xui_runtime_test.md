# Xui runtime test: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/tests/xui_runtime_test.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 95

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L95)

```text
// A skin with a button visual (100x20) and a label visual named after its class.
```

## Source note 2, line 188

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L188)

```text
// The visual is 100 wide; the button is 150.
```

## Source note 3, line 193

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L193)

```text
// No Visual property: the visual named after the class.
```

## Source note 4, line 207

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L207)

```text
// Halfway through an ease-in -100 / ease-out 100 segment.
```

## Source note 5, line 211

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L211)

```text
// EndFocus stops the playhead on its frame.
```

## Source note 6, line 239

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L239)

```text
// The skin document is untouched.
```

## Source note 7, line 272

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L272)

```text
// a, b, c stacked 20 apart, linked up and down.
```

## Source note 8, line 306

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L306)

```text
// the model's visual
```

## Source note 9, line 308

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L308)

```text
// A plain entry made from a row of another look.
```

## Source note 10, line 319

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L319)

```text
// Five entries 20 tall in a scene 50 tall: two and a half show.
```

## Source note 11, line 411

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L411)

```text
// Local only (REXGLUE_SYSTEM_UPDATE): the console's guide plays its blade shuffle.
```

## Source note 12, line 448

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L448)

```text
// A GuideMain whose Tabscene tracks hold each label's (or blade's) number
```

## Source note 13, line 449

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L449)

```text
// plus the frame, at the frames the three-tab rewrite reads.
```

## Source note 14, line 494

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L494)

```text
// Label number (the track's index in MakeGuideMain) and Show at `frame`.
```

## Source note 15, line 512

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L512)

```text
// Tabs 1 and 2: Settings is where Media was; the third right blade is gone.
```

## Source note 16, line 518

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L518)

```text
// Tab 4: Games where Home was, Home where Media was; the third left blade
```

## Source note 17, line 519

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L519)

```text
// is gone; Home's content and selected label leave as Media's did.
```

## Source note 18, line 526

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L526)

```text
// Tab3
```

## Source note 19, line 528

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L528)

```text
// its own outside the switches
```

## Source note 20, line 529

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L529)

```text
// Untouched outside the rewritten frames.
```

## Source note 21, line 548

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L548)

```text
// Local only (REXGLUE_SYSTEM_UPDATE): the console's guide, rewritten for three
```

## Source note 22, line 549

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L549)

```text
// tabs, rests with its blades and labels where a three-tab guide has them.
```

## Source note 23, line 577

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L577)

```text
// the guide opening on Home
```

## Source note 24, line 604

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L604)

```text
// Local only (REXGLUE_GUIDE_FLASH, an Xbox PC backward-compatibility game's
```

## Source note 25, line 605

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L605)

```text
// Content/Flash): the guide Microsoft's backward compatibility shows, with its
```

## Source note 26, line 606

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L606)

```text
// own three tabs.
```

## Source note 27, line 673

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L673)

```text
// A BC-only HUD may lack the ordinary scenes. Never substitute its emulator
```

## Source note 28, line 674

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L674)

```text
// scenes when a 360 host loads it through the default API.
```

## Source note 29, line 685

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L685)

```text
// 30 units over, 10 a second: 2.5 s at the top, 3 s scrolling, 2 s at the
```

## Source note 30, line 686

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L686)

```text
// end, 0.6 s fading out; then it fades back in at the top.
```

## Source note 31, line 688

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L688)

```text
// no fade the first time
```

## Source note 32, line 696

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_runtime_test.cpp#L696)

```text
// Text that fits never moves.
```
