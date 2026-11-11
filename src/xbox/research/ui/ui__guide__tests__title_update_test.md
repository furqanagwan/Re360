# Title update test: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/tests/title_update_test.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L36)

```text
// A LIVE title update header like Quantum of Solace's title update 2: title
```

## Source note 2, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L37)

```text
// 415607FF, media 06DD88A0, base version 7, header size 0xAD0E, whose content
```

## Source note 3, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L38)

```text
// ID is the SHA-1 of 0x344 to 0xB000.
```

## Source note 4, line 102

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L102)

```text
// inside the hashed header region
```

## Source note 5, line 118

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L118)

```text
// other game
```

## Source note 6, line 134

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L134)

```text
// TitleUpdateInfo.php?titleid=415607FF, 2026-10-01.
```

## Source note 7, line 144

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L144)

```text
// 007 Legends has none.
```

## Source note 8, line 174

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L174)

```text
// A wrong package is refused and leaves the installed one alone.
```

## Source note 9, line 182

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L182)

```text
// A finished download (<name>.part) is installed under its own name, alone.
```

## Source note 10, line 233

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L233)

```text
// Off: the original runs.
```

## Source note 11, line 238

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L238)

```text
// On, but not installed or not built: the original runs anyway.
```

## Source note 12, line 244

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L244)

```text
// no qos_tu2.exe yet
```

## Source note 13, line 246

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L246)

```text
// On, installed and built: hand over to the update build...
```

## Source note 14, line 251

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L251)

```text
// ...which runs with the installed package.
```

## Source note 15, line 256

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L256)

```text
// Turned off: the update build hands back to the original.
```

## Source note 16, line 260

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L260)

```text
// Never twice: a handed-over build runs as it is.
```

## Source note 17, line 264

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L264)

```text
// The update removed: the update build goes back to the original.
```

## Source note 18, line 274

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/title_update_test.cpp#L274)

```text
// Reaches xboxunity.net, so it runs only when asked for: unit_tests "[.network]".
```
