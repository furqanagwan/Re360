# Title update: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/title_update.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 73

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L73)

```text
// An open GET request whose response headers have arrived.
```

## Source note 2, line 77

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L77)

```text
// Content-Length, 0 when absent
```

## Source note 3, line 176

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L176)

```text
// Display name: UTF-16BE at 0x411, 0x80 bytes per language; English first.
```

## Source note 4, line 264

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L264)

```text
// A download arrives as <name>.part; a picked file keeps its own name.
```

## Source note 5, line 275

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L275)

```text
// Only the new copy stays: one package per version.
```

## Source note 6, line 454

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L454)

```text
// outlives exit
```

## Source note 7, line 458

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L458)

```text
// Runs `work` for a new job of `update`, unless one is running for it.
```

## Source note 8, line 546

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L546)

```text
// The update the player turned on runs only if it's installed and built.
```

## Source note 9, line 563

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L563)

```text
// Handed an update build without its update: go back to the original.
```

## Source note 10, line 577

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/title_update.cpp#L577)

```text
// Only an update build can lack the original beside it; run as built.
```
