# Text scroll: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/text_scroll.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 15

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L15)

```text
/// A wrapped text taller than its box scrolls as the console's long
```

## Source note 2, line 16

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L16)

```text
/// descriptions did: it holds at the top, scrolls slowly to the end, holds,
```

## Source note 3, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L17)

```text
/// fades out, and fades back in at the top.
```

## Source note 4, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L19)

```text
///< what it showed when `start` was taken
```

## Source note 5, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L20)

```text
///< seconds (ImGui::GetTime)
```

## Source note 6, line 23

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L23)

```text
///< how far the text has moved up, in its own units
```

## Source note 7, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L24)

```text
///< its opacity in the fade
```

## Source note 8, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L26)

```text
/// Where a text `overflow` taller than its box is `elapsed` seconds into the
```

## Source note 9, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/text_scroll.h#L27)

```text
/// cycle, scrolling `speed` units a second.
```
