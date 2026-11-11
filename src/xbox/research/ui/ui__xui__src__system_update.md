# System update: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/system_update.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L26)

```text
// "XEX2"
```

## Source note 2, line 119

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L119)

```text
// basic: (data size, zero size) blocks
```

## Source note 3, line 133

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L133)

```text
// normal: hashed blocks of LZX chunks
```

## Source note 4, line 185

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L185)

```text
// Entries: 8-byte name, image address, size.
```

## Source note 5, line 213

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L213)

```text
// Only XUIZ packages; XEXs also carry icons, XDBF and sounds.
```

## Source note 6, line 275

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L275)

```text
// A font that does not convert leaves the guide on its fallback font.
```

## Source note 7, line 296

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L296)

```text
// "RXGU", little-endian
```

## Source note 8, line 386

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L386)

```text
// A file named $flash_<name> in a flash folder, or <name> as Xbox PC
```

## Source note 9, line 387

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L387)

```text
// backward-compatibility games ship them.
```

## Source note 10, line 407

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L407)

```text
// The update folder holds some fonts beside its package.
```

## Source note 11, line 409

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/system_update.cpp#L409)

```text
// The update package is named su<version>_00000000.
```
