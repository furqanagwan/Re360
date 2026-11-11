# Xbox keyboard: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/xbox_keyboard.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 30

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L30)

```text
/// The keyboard a title opens with XamShowKeyboardUI, from the console's own
```

## Source note 2, line 31

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L31)

```text
/// scenes (vk KeyboardMain hosting KeyboardBase, in the HUD backdrop as XAM
```

## Source note 3, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L32)

```text
/// shows it). Owns itself as XAM dialogs do: `done` gets the text, or nothing
```

## Source note 4, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L33)

```text
/// when cancelled, before it deletes itself after its close animation.
```

## Source note 5, line 51

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L51)

```text
// A spot on the keyboard: rows 0-4 are the keys, 5 Backspace and Space;
```

## Source note 6, line 52

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L52)

```text
// column -1 is the left column, 10 the right.
```

## Source note 7, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L81)

```text
// PC keys wait until every key held as the keyboard opened (the Enter that
```

## Source note 8, line 82

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/xbox_keyboard.h#L82)

```text
// asked for it) is released, as pad buttons do.
```
