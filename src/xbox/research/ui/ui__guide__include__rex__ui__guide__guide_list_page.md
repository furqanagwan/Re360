# Guide list page: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/guide_list_page.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 14

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L14)

```text
// right-aligned, as Manage Storage shows a save's size
```

## Source note 2, line 15

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L15)

```text
// the right pane while this row has focus
```

## Source note 3, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L18)

```text
// A Guide page outside the running guide: the HUD frame at full height
```

## Source note 4, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L19)

```text
// hosting the Options scene as a list, as Manage Storage and Active Downloads
```

## Source note 5, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L20)

```text
// look in the in-game Guide. No runtime or guest dependencies; the caller
```

## Source note 6, line 21

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L21)

```text
// supplies input, rendering resources and actions. The assets and the
```

## Source note 7, line 22

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L22)

```text
// contexts' skin must outlive this object.
```

## Source note 8, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L27)

```text
// the list area of the Options scene
```

## Source note 9, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L32)

```text
// `frame` is GuideAssets::backdrop (context package xam/xam) and `options`
```

## Source note 10, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L33)

```text
// GuideAssets::options_notifications (hud/hud). Null when either lacks the
```

## Source note 11, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L34)

```text
// controls this uses. The caller hides the legend glyphs' letters
```

## Source note 12, line 35

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L35)

```text
// (HideGuideButtonLetters) when it draws them itself.
```

## Source note 13, line 41

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L41)

```text
// Replaces the page's contents; `empty_details` fills the right pane when
```

## Source note 14, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L42)

```text
// there are no rows. The frame's open animation is not replayed.
```

## Source note 15, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L46)

```text
// Up (-1) or down (+1), scrolling the visible window; stops at the ends.
```

## Source note 16, line 49

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L49)

```text
// The focused row, pressed, unless there are none.
```

## Source note 17, line 54

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L54)

```text
// The visible row control at `slot` (0 to kVisibleRows - 1), or null.
```

## Source note 18, line 61

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_list_page.h#L61)

```text
// Plays the frame's open animation (on creation) or close animation.
```
