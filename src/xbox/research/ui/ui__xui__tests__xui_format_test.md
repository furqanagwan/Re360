# Xui format test: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/tests/xui_format_test.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 28

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L28)

```text
// Writes XUR v8 the way the console's files are laid out: pools, then an
```

## Source note 2, line 29

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L29)

```text
// element tree referring into them.
```

## Source note 3, line 124

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L124)

```text
// A canvas holding a text, a gradient-filled figure and a second text that
```

## Source note 4, line 125

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L125)

```text
// shares the first's properties, with a timeline for each of the first two.
```

## Source note 5, line 128

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L128)

```text
// Canvas: Width, Height (XuiElement bits 1 and 2), children, timelines.
```

## Source note 6, line 139

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L139)

```text
// XuiElement: Id
```

## Source note 7, line 141

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L141)

```text
// XuiText: Text, TextColor, PointSize
```

## Source note 8, line 150

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L150)

```text
// XuiFigure: Fill
```

## Source note 9, line 151

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L151)

```text
// a new compound
```

## Source note 10, line 153

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L153)

```text
// FillType, FillColor, Gradient
```

## Source note 11, line 156

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L156)

```text
// a new compound
```

## Source note 12, line 157

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L157)

```text
// NumStops + two StopColor + two StopPos
```

## Source note 13, line 168

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L168)

```text
// the label's properties (bag 0 is the canvas's)
```

## Source note 14, line 170

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L170)

```text
// Named frames and timelines of the canvas.
```

## Source note 15, line 176

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L176)

```text
// go to and stop
```

## Source note 16, line 178

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L178)

```text
// named frames
```

## Source note 17, line 182

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L182)

```text
// label.Opacity: XuiElement is class 1 of [XuiText, XuiElement].
```

## Source note 18, line 185

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L185)

```text
// depth 1
```

## Source note 19, line 190

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L190)

```text
// fig.Fill.FillColor.
```

## Source note 20, line 193

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L193)

```text
// depth 2
```

## Source note 21, line 292

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L292)

```text
// Shared properties are the same bag, not a copy.
```

## Source note 22, line 342

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L342)

```text
// A property bit past XuiGroup's (empty) list.
```

## Source note 23, line 346

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L346)

```text
// XuiElement: Id
```

## Source note 24, line 349

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L349)

```text
// the unknown property
```

## Source note 25, line 388

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L388)

```text
// Non-XUIZ resources (icons, XDBF) are not packages.
```

## Source note 26, line 403

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L403)

```text
// The guide needs xam/shrdres too: a bundle without it is refused.
```

## Source note 27, line 407

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L407)

```text
// One resource per module in this synthetic XEX, so xam/skin goes missing.
```

## Source note 28, line 410

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L410)

```text
// Truncated or foreign data is refused, never read past its end.
```

## Source note 29, line 422

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L422)

```text
/*encryption=*/
```

## Source note 30, line 426

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L426)

```text
// Local only: REXGLUE_SYSTEM_UPDATE names the owner's $SystemUpdate folder
```

## Source note 31, line 427

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L427)

```text
// (dashboard 2.0.17559). Decodes every scene the guide uses.
```

## Source note 32, line 456

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L456)

```text
// Every scene in those packages except the Kinect ones (nuihud, Nui*),
```

## Source note 33, line 457

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L457)

```text
// whose ControlPack classes the guide does not use.
```

## Source note 34, line 495

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xui_format_test.cpp#L495)

```text
// 2.0.17559 carries its light Segoe in the package (RG-GDK-061).
```
