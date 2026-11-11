# Runtime: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/runtime.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L20)

```text
// XuiElement's defaults for properties a scene leaves out.
```

## Source note 2, line 58

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L58)

```text
// Normalized lerp along the shorter arc.
```

## Source note 3, line 70

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L70)

```text
// Bools, integers and strings hold until the next keyframe.
```

## Source note 4, line 74

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L74)

```text
// A property's value at `frame`: the keyframe at or before it, interpolated
```

## Source note 5, line 75

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L75)

```text
// toward the next by the earlier keyframe's mode.
```

## Source note 6, line 102

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L102)

```text
// A cubic whose inner control values follow the ease settings: 0/0 is
```

## Source note 7, line 103

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L103)

```text
// linear, a negative ease-in starts fast, a positive ease-out ends slow.
```

## Source note 8, line 134

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L134)

```text
// Controls take their visual's elements and timelines from the skin: the
```

## Source note 9, line 135

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L135)

```text
// named Visual, or the one named after the class.
```

## Source note 10, line 157

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L157)

```text
// Controls rest in their visual's Normal state, as XUI starts them.
```

## Source note 11, line 159

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L159)

```text
/*sounds=*/
```

## Source note 12, line 373

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L373)

```text
// Returns a copy of `bag` with the value at `path` (and array `index`) replaced.
```

## Source note 13, line 423

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L423)

```text
// Compound or indexed: replace a copy, since bags are shared with the scene.
```

## Source note 14, line 511

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L511)

```text
/*inclusive=*/
```

## Source note 15, line 529

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L529)

```text
// The first command the playhead crosses decides where it ends up.
```

## Source note 16, line 552

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L552)

```text
/*inclusive=*/
```

## Source note 17, line 590

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L590)

```text
// unknown to the schema, or a sound cue
```

## Source note 18, line 625

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L625)

```text
// Labels are controls in the class tree but never take focus.
```

## Source note 19, line 630

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L630)

```text
/*sounds=*/
```

## Source note 20, line 648

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L648)

```text
/*sounds=*/
```

## Source note 21, line 660

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L660)

```text
// Hidden or disabled controls pass navigation on in the same direction.
```

## Source note 22, line 682

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L682)

```text
// Checked or disabled controls have no KillFocus variant: they go
```

## Source note 23, line 683

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L683)

```text
// straight back to their resting state.
```

## Source note 24, line 685

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L685)

```text
/*sounds=*/
```

## Source note 25, line 690

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L690)

```text
/*sounds=*/
```

## Source note 26, line 691

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/runtime.cpp#L691)

```text
/*sounds=*/
```
