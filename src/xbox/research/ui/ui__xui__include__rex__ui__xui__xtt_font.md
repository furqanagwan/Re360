# Xtt font: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/xtt_font.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L19)

```text
/// Converts one of the console's .xtt fonts (xenonjklatin.xtt,
```

## Source note 2, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L20)

```text
/// SegoeXbox-Light.xtt...) to a TrueType font that ImGui and FreeType load.
```

## Source note 3, line 22

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L22)

```text
/// An XTT file is a signed header and a zlib-compressed sfnt directory whose
```

## Source note 4, line 23

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L23)

```text
/// outlines live in Xbox tables: `xglf` holds the glyphs in separately
```

## Source note 5, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L24)

```text
/// compressed 4 KiB blocks, `xloc` locates each glyph as (block << 16) |
```

## Source note 6, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L25)

```text
/// offset, and `xchk` holds a SHA-1 per block. The result keeps the font's
```

## Source note 7, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L26)

```text
/// own standard tables (cmap, head, hhea, hmtx, name...), rebuilds glyf and
```

## Source note 8, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L27)

```text
/// loca from the blocks, and derives maxp, OS/2 and post when the font has
```

## Source note 9, line 28

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/xtt_font.h#L28)

```text
/// none.
```
