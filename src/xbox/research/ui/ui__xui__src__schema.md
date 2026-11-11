# Schema: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/schema.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 11

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L11)

```text
// XUR v8 writes one property mask per class, base class first, bit N meaning
```

## Source note 2, line 12

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L12)

```text
// the class's Nth property. The order below is interoperability data: it is
```

## Source note 3, line 13

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L13)

```text
// what the console's XUI runtime numbers. It covers the classes the 17559
```

## Source note 4, line 14

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L14)

```text
// guide, skin and achievement scenes use, and was checked by decoding every
```

## Source note 5, line 15

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L15)

```text
// one of those scenes to its declared object count with the DATA section fully
```

## Source note 6, line 16

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L16)

```text
// consumed. docs/xbox-guide.md records how.
```

## Source note 7, line 266

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L266)

```text
// A dash command button: SettingsTabSignedIn writes a DashCommand mask
```

## Source note 8, line 267

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L267)

```text
// level between XuiNavButton's and its own.
```

## Source note 9, line 285

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/schema.cpp#L285)

```text
// Compound property classes, reached through Fill, Gradient and Stroke.
```
