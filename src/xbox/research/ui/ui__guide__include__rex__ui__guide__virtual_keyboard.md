# Virtual keyboard: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/virtual_keyboard.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L17)

```text
/// The text a title asked for through XamShowKeyboardUI, edited as on the
```

## Source note 2, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L18)

```text
/// console's keyboard (vk.xex): a 5 x 10 grid of keys on one of three pages
```

## Source note 3, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L19)

```text
/// (the English layouts of the 2.0.17559 keyboard: QWERTY letters, symbols,
```

## Source note 4, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L20)

```text
/// accented letters), Caps, a cursor, Backspace and Space.
```

## Source note 5, line 29

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L29)

```text
/// `max_length` characters at most (the guest buffer less its terminator).
```

## Source note 6, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L32)

```text
/// The character the key at `row`, `column` types now (Caps applied);
```

## Source note 7, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L33)

```text
/// 0 for no key.
```

## Source note 8, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L37)

```text
/// The page the previous (LT) and next (RT) page keys go to.
```

## Source note 9, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L42)

```text
/// Types the key at `row`, `column`; false when it has no character or the
```

## Source note 10, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L43)

```text
/// text is full.
```

## Source note 11, line 48

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/virtual_keyboard.h#L48)

```text
/// Moves the cursor by `delta` characters, within the text.
```
