# Virtual keyboard: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/virtual_keyboard.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L18)

```text
// The English pages of the 2.0.17559 keyboard, row by row, as vk.xex lays
```

## Source note 2, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L19)

```text
// them out (its tables give "-" and "." as the virtual keys 0xBD and 0xBE).
```

## Source note 3, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L20)

```text
// The letters have four rows; the fifth is blank.
```

## Source note 4, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L42)

```text
// The capital of a letter on the keyboard's pages (Basic Latin, Latin-1 and
```

## Source note 5, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L43)

```text
// Latin Extended-A); `c` when it has none (ß, µ). towupper only maps ASCII in
```

## Source note 6, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/virtual_keyboard.cpp#L44)

```text
// the C locale.
```
