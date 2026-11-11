# Runtime: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/runtime.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L26)

```text
/// XUI timelines count frames at this rate (inferred; see docs/xbox-guide.md).
```

## Source note 2, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L42)

```text
/// Resolves a scene path ("sharedres://A-Button.png", "xam://x.png" or a
```

## Source note 3, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L43)

```text
/// name relative to `base_package`) to a file in the system update. Relative
```

## Source note 4, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L44)

```text
/// names not in the base package are looked up in the skin and shared
```

## Source note 5, line 45

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L45)

```text
/// packages, where skin visuals keep their images. Empty when missing.
```

## Source note 6, line 49

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L49)

```text
/// The cubic ease XUI keyframes use, for signed -100..100 in/out values.
```

## Source note 7, line 52

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L52)

```text
/// Where a scene's relative paths resolve and the skin it takes visuals from.
```

## Source note 8, line 54

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L54)

```text
// skin.xur; visuals are its root's XuiVisual children
```

## Source note 9, line 55

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L55)

```text
// "hud/hud": base for relative paths
```

## Source note 10, line 56

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L56)

```text
/// Called for each sound cue a playing timeline crosses.
```

## Source note 11, line 62

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L62)

```text
/// Builds `node` and its subtree, applying skin visuals to controls, and
```

## Source note 12, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L63)

```text
/// shows every timeline's frame 0.
```

## Source note 13, line 72

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L72)

```text
/// True when the element's class is `name` or derives from it.
```

## Source note 14, line 76

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L76)

```text
/// Depth-first search of the descendants by Id.
```

## Source note 15, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L81)

```text
// Properties: the scene's values, then whatever timelines and code set.
```

## Source note 16, line 91

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L91)

```text
/// Compound property (Fill, Stroke), with its own members.
```

## Source note 17, line 97

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L97)

```text
/// A second label some visuals show (text_Label2: counts, values).
```

## Source note 18, line 100

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L100)

```text
/// Shown, and not suppressed.
```

## Source note 19, line 103

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L103)

```text
/// Hidden whatever its timelines say: for entries a host leaves out.
```

## Source note 20, line 107

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L107)

```text
/// Size after anchoring against the parent's current size.
```

## Source note 21, line 110

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L110)

```text
/// Position after anchoring (the element's top-left in its parent).
```

## Source note 22, line 113

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L113)

```text
// Timelines: an element's timelines animate its descendants.
```

## Source note 23, line 115

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L115)

```text
/// Jumps to the named frame and plays until a stop command. False when the
```

## Source note 24, line 116

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L116)

```text
/// element has no such frame.
```

## Source note 25, line 120

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L120)

```text
/// Shows frame `frame` of this element's timelines and stops there (a
```

## Source note 26, line 121

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L121)

```text
/// slider's body is posed by its value this way).
```

## Source note 27, line 123

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L123)

```text
/// Advances this element's and all descendants' timelines.
```

## Source note 28, line 126

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L126)

```text
/// Adds a scene built from `node` (a tab's content file) as a child.
```

## Source note 29, line 128

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L128)

```text
/// A copy of `source` (a child of this element) built from the same scene
```

## Source note 30, line 129

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L129)

```text
/// data, placed after it: new menu entries look exactly like the others.
```

## Source note 31, line 130

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L130)

```text
/// `visual` names a different skin visual for the copy.
```

## Source note 32, line 134

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L134)

```text
/// Fills a list with `count` copies of its visual's item template
```

## Source note 33, line 135

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L135)

```text
/// (control_ListItem), `columns` per row. Replaces earlier items.
```

## Source note 34, line 139

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L139)

```text
// Focus: shown controls (not scenes). Disabled ones take focus too, and
```

## Source note 35, line 140

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L140)

```text
// play their visual's Disable frames, as on the console.
```

## Source note 36, line 143

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L143)

```text
/// Plays Press (PressCheck when checked), or PressDisable when disabled.
```

## Source note 37, line 145

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L145)

```text
/// Checkboxes and radio buttons: plays the visual's Check (or plain)
```

## Source note 38, line 146

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L146)

```text
/// state for the control's focus.
```

## Source note 39, line 150

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L150)

```text
/// The control Nav<direction> names, searched in the enclosing scene.
```

## Source note 40, line 152

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L152)

```text
/// Plays the visual's KillFocus on `from` and Focus (or InitFocus when
```

## Source note 41, line 153

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L153)

```text
/// `initial`) on `to`, with the Check and Disable variants that apply.
```

## Source note 42, line 170

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L170)

```text
/// Plays `base` + "Check" when checked + "Disable" when disabled, falling
```

## Source note 43, line 171

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L171)

```text
/// back to fewer suffixes when the visual lacks that frame.
```

## Source note 44, line 177

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L177)

```text
// for AttachScene'd content
```

## Source note 45, line 184

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/runtime.h#L184)

```text
// Size the parent had when this element was laid out, for anchoring.
```
