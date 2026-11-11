# Guide layout: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/guide_layout.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 23

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L23)

```text
// The Tabscene's frames for each layout (17559 GuideMain named frames).
```

## Source note 2, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L24)

```text
// 1To2, 2To1; 1Open to 2CloseEnd
```

## Source note 3, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L25)

```text
// 3To4, 4To3; 4Open to 4CloseEnd
```

## Source note 4, line 52

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L52)

```text
// `target`'s keyframes in `ranges` replaced by `source`'s there.
```

## Source note 5, line 66

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L66)

```text
// `track`'s Show false at every keyframe in `ranges`.
```

## Source note 6, line 88

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L88)

```text
// Copies of the scene's own tracks, so a label that moves to its
```

## Source note 7, line 89

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L89)

```text
// neighbour's track is read before that neighbour is changed.
```

## Source note 8, line 132

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_layout.cpp#L132)

```text
// Home to Settings is 3To4's frames; 2To3 and 3To2 have no use.
```
