# Document: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/document.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L34)

```text
/// 0xAARRGGBB.
```

## Source note 2, line 40

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L40)

```text
/// A figure outline: cubic Bezier segments in the figure's own box.
```

## Source note 3, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L44)

```text
// control point before the anchor
```

## Source note 4, line 45

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L45)

```text
// control point after the anchor
```

## Source note 5, line 68

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L68)

```text
/// An element's (or a compound property's) set properties.
```

## Source note 6, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L71)

```text
// nullptr: a property the schema does not know
```

## Source note 7, line 84

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L84)

```text
// -100..100
```

## Source note 8, line 87

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L87)

```text
// one per Timeline::props entry
```

## Source note 9, line 90

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L90)

```text
/// One animated property: a path through compound properties (Fill, then
```

## Source note 10, line 91

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L91)

```text
/// FillColor) ending at the animated one, and the array element for
```

## Source note 11, line 92

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L92)

```text
/// indexed properties.
```

## Source note 12, line 94

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L94)

```text
// empty: not in the schema, ignored
```

## Source note 13, line 110

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L110)

```text
// the named frame the GoTo commands jump to
```

## Source note 14, line 118

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L118)

```text
// animate this node's descendants
```

## Source note 15, line 123

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L123)

```text
/// Depth-first search of the descendants (not this node) by Id.
```

## Source note 16, line 128

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/document.h#L128)

```text
// an XuiCanvas
```
