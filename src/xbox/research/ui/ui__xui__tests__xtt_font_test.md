# Xtt font test: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/tests/xtt_font_test.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L46)

```text
// zlib with one stored (uncompressed) deflate block.
```

## Source note 2, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L63)

```text
// A simple glyph: one contour of `points` on-curve points.
```

## Source note 3, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L71)

```text
// no instructions
```

## Source note 4, line 73

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L73)

```text
// on curve, short x and y
```

## Source note 5, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L81)

```text
// An XTT font of three glyphs; glyphs 0 and 1 share the first block and
```

## Source note 6, line 82

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L82)

```text
// glyph 2 starts the second, as the console's fonts are laid out.
```

## Source note 7, line 102

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L102)

```text
// unitsPerEm 2048
```

## Source note 8, line 105

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L105)

```text
// ascender 2048
```

## Source note 9, line 111

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L111)

```text
// The directory: tables after it, xglf at its place in the file.
```

## Source note 10, line 175

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L175)

```text
// loca (32-bit, as head says) places the glyphs back to back, each aligned.
```

## Source note 11, line 184

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L184)

```text
// glyph 2's last point: 5 points
```

## Source note 12, line 185

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L185)

```text
// glyph 1: 4 points
```

## Source note 13, line 191

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L191)

```text
// The whole font sums to the magic number once head is adjusted.
```

## Source note 14, line 205

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L205)

```text
// the glyph blocks are cut off
```

## Source note 15, line 224

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L224)

```text
// An Xbox PC backward-compatibility game's Content/Flash folder and/or the
```

## Source note 16, line 225

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/tests/xtt_font_test.cpp#L225)

```text
// console's $SystemUpdate (never in the repository).
```
