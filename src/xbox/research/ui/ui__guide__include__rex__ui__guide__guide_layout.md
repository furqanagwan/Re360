# Guide layout: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/guide_layout.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L19)

```text
/// The first child of a scene file's canvas: the scene itself.
```

## Source note 2, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L24)

```text
/// Rewrites GuideMain (its Tabscene's timelines) for three tabs, Games & Apps
```

## Source note 3, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L25)

```text
/// (1), Home (2) and Settings (4), without Media (3). Every switch moves each
```

## Source note 4, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L26)

```text
/// blade one slot, so a side with one tab fewer is the same motion with its
```

## Source note 5, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L27)

```text
/// outermost blade hidden and each label on its inner neighbour's track:
```

## Source note 6, line 28

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L28)

```text
/// - tabs 1 and 2 (1To2, 2To1, 1Open to 2Close): Blade5, the third blade on
```

## Source note 7, line 29

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L29)

```text
///   the right, is hidden and txt_Settings follows txt_Media;
```

## Source note 8, line 30

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L30)

```text
/// - tab 4: Home to Settings and back play 3To4 and 4To3, renamed 2To4 and
```

## Source note 9, line 31

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L31)

```text
///   4To2. Blade6, the third on the left, is hidden; txt_Games follows
```

## Source note 10, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L32)

```text
///   txt_home and txt_home txt_Media; Home's content (Tab2) and selected label
```

## Source note 11, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L33)

```text
///   (txt_homeSel) fade as Media's did. 4Open and 4Close keep Blade6 hidden
```

## Source note 12, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L34)

```text
///   and the labels moved.
```

## Source note 13, line 35

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L35)

```text
/// 2To3 and 3To2 are dropped. Returns false (scene untouched) when GuideMain
```

## Source note 14, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L36)

```text
/// is not the 17559 layout this expects.
```

## Source note 15, line 39

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L39)

```text
/// Hides `id` in `scene` for good and closes the gap: entries below it move
```

## Source note 16, line 40

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L40)

```text
/// up by its height, and the neighbours' NavUp/NavDown skip it.
```

## Source note 17, line 71

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L71)

```text
/// Adds an entry below `after` that is a copy of `model` (same class and
```

## Source note 18, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L72)

```text
/// visual, so it looks like its neighbours, or `visual` when given), with
```

## Source note 19, line 73

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L73)

```text
/// `text`. Entries below `after` move down to make room.
```

## Source note 20, line 107

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L107)

```text
/// Scrolls the menu holding `entry` by whole entries until `entry` lies inside
```

## Source note 21, line 108

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L108)

```text
/// the menu scene's height. A menu with entries past that height is clipped to
```

## Source note 22, line 109

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_layout.h#L109)

```text
/// it, so the entries scrolled out of view are not drawn over the blade.
```
