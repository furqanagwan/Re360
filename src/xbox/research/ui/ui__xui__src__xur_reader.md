# Xur reader: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/xur_reader.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L17)

```text
// Layout (all big-endian):
```

## Source note 2, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L18)

```text
//   header: "XUIB", version 8, flags, tool version (u16), file size, section
```

## Source note 3, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L19)

```text
//     count (u16)
```

## Source note 4, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L20)

```text
//   count header: 12 packed integers (totals; [0] is the object count)
```

## Source note 5, line 21

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L21)

```text
//   section table: (magic, offset, length) per section
```

## Source note 6, line 22

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L22)

```text
//   STRN strings, VECT/QUAT/FLOT/COLR value pools, CUST figure paths,
```

## Source note 7, line 23

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L23)

```text
//   KEYP keyframe values, KEYD keyframes, NAME named frames, DATA the element
```

## Source note 8, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L24)

```text
//   tree. Properties, keyframe values and named frames refer into the pools
```

## Source note 9, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L25)

```text
//   by index.
```

## Source note 10, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L26)

```text
// A packed integer is one byte below 0xF0, 0xFnnn in two bytes, or 0xFF and a
```

## Source note 11, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L27)

```text
// 32-bit value.
```

## Source note 12, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L62)

```text
// "XUIB"
```

## Source note 13, line 139

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L139)

```text
// first KEYP entry
```

## Source note 14, line 199

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L199)

```text
// tool version
```

## Source note 15, line 210

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L210)

```text
// The section table follows the count header.
```

## Source note 16, line 237

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L237)

```text
// Pools first: STRN before everything that names strings.
```

## Source note 17, line 239

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L239)

```text
// index 0 is the empty string
```

## Source note 18, line 243

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L243)

```text
// total length
```

## Source note 19, line 366

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L366)

```text
// Every node is counted; a schema mismatch shows up here first.
```

## Source note 20, line 397

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L397)

```text
// Identical to an earlier element's properties.
```

## Source note 21, line 426

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L426)

```text
// Timelines animate descendants, so a leaf has no timeline count.
```

## Source note 22, line 474

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L474)

```text
// Properties past the schema's end: every non-compound, non-indexed type is
```

## Source note 23, line 475

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L475)

```text
// one packed integer (a bool's byte reads the same), so skip them as that.
```

## Source note 24, line 533

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L533)

```text
// A compound value is written once and referred to by index after.
```

## Source note 25, line 613

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L613)

```text
// Per figure: data length, bounding box, point count, then per point the
```

## Source note 26, line 614

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L614)

```text
// anchor and two control points.
```

## Source note 27, line 647

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L647)

```text
// Property paths are numbered from the target's own class down to
```

## Source note 28, line 648

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xur_reader.cpp#L648)

```text
// XuiElement. A target the tree does not hold still has to be read past.
```
