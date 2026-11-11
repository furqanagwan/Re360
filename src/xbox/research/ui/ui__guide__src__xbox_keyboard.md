# Xbox keyboard: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/xbox_keyboard.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L37)

```text
// The title behind is darkened as for the guide, over the backdrop's
```

## Source note 2, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L38)

```text
// ClosedToFull and FullToClosed.
```

## Source note 3, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L43)

```text
// The side columns, top to bottom, each key two rows tall.
```

## Source note 4, line 60

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L60)

```text
// Buttons already held (the A that opened the title's prompt) wait for
```

## Source note 5, line 61

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L61)

```text
// their release.
```

## Source note 6, line 97

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L97)

```text
// The title and description the title passed.
```

## Source note 7, line 104

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L104)

```text
// The Latin keyboard: the Japanese keys, conversion candidates and kana
```

## Source note 8, line 105

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L105)

```text
// mode are for the Japanese one.
```

## Source note 9, line 119

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L119)

```text
// The side keys' pictures (vk's own) and words.
```

## Source note 10, line 138

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L138)

```text
// The legends, as the console shows them under the keyboard.
```

## Source note 11, line 155

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L155)

```text
// q, as the console starts on the first letter
```

## Source note 12, line 156

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L156)

```text
/*initial=*/
```

## Source note 13, line 179

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L179)

```text
// Across: the side columns are one more column each end; it wraps.
```

## Source note 14, line 185

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L185)

```text
// Keep moving over the cells of a wide or tall key.
```

## Source note 15, line 218

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L218)

```text
// The edit visual's own caret (scr_Edit: its text at x 6, the caret 4
```

## Source note 16, line 219

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L219)

```text
// further in when empty) after the characters before the cursor.
```

## Source note 17, line 254

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L254)

```text
/*accepted=*/
```

## Source note 18, line 280

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L280)

```text
/*accepted=*/
```

## Source note 19, line 311

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L311)

```text
/*accepted=*/
```

## Source note 20, line 331

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L331)

```text
// Not on the first frame: a press that opened the keyboard still reads as
```

## Source note 21, line 332

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L332)

```text
// pressed in it.
```

## Source note 22, line 347

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L347)

```text
// A PC keyboard types into the field; Enter is Done, Escape cancels and
```

## Source note 23, line 348

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L348)

```text
// the arrows move over the keys.
```

## Source note 24, line 355

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L355)

```text
/*repeat=*/
```

## Source note 25, line 370

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L370)

```text
/*repeat=*/
```

## Source note 26, line 394

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L394)

```text
// Typed characters arrive while text input is on, as for an ImGui text box.
```

## Source note 27, line 396

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/xbox_keyboard.cpp#L396)

```text
// The caret blinks, shown again on each edit.
```
