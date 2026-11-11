# Xtt font: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/xtt_font.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L18)

```text
// The zlib decoder of stb_image; its implementation is in ui/image_decode.cpp.
```

## Source note 2, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L24)

```text
// "xttf"
```

## Source note 3, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L25)

```text
// The header: magic, a 256-byte signature, then the signed, file, compressed
```

## Source note 4, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L26)

```text
// and uncompressed sizes and the version. The compressed directory follows.
```

## Source note 5, line 61

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L61)

```text
/// zlib-inflates `in` (with its header); trailing bytes after the stream are
```

## Source note 6, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L62)

```text
/// ignored, as the glyph blocks are padded.
```

## Source note 7, line 88

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L88)

```text
/// maxp 1.0 sized from the outlines: points and contours per simple glyph,
```

## Source note 8, line 89

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L89)

```text
/// components per composite one (one level deep, as these fonts are).
```

## Source note 9, line 155

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L155)

```text
/// OS/2 version 4 from the horizontal metrics; Windows and FreeType read
```

## Source note 10, line 156

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L156)

```text
/// their line metrics from it.
```

## Source note 11, line 165

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L165)

```text
// xAvgCharWidth: half an em
```

## Source note 12, line 168

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L168)

```text
// fsType: installable
```

## Source note 13, line 170

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L170)

```text
// sub/superscript, strikeout, class, PANOSE, ranges
```

## Source note 14, line 172

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L172)

```text
// fsSelection: regular
```

## Source note 15, line 180

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L180)

```text
// ulCodePageRange1: Latin 1
```

## Source note 16, line 181

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L181)

```text
// ulCodePageRange2
```

## Source note 17, line 190

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L190)

```text
/// post version 3: metrics only, no glyph names.
```

## Source note 18, line 222

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L222)

```text
// Table offsets are into the directory, except xglf's, which is into the file.
```

## Source note 19, line 248

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L248)

```text
// Each glyph sits inside one compressed block; blocks are inflated once.
```

## Source note 20, line 290

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L290)

```text
// checkSumAdjustment, set below
```

## Source note 21, line 291

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L291)

```text
// indexToLocFormat: 32-bit loca
```

## Source note 22, line 301

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L301)

```text
// The font's own standard tables are kept; those it leaves out are built.
```

## Source note 23, line 322

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/xtt_font.cpp#L322)

```text
// The sfnt: tables in tag order, each 4-byte aligned.
```
