# Package: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/package.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 31

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L31)

```text
// XUIZ header: magic, version, package size, reserved, entry table size,
```

## Source note 2, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L32)

```text
// entry count. Entries follow at kTableStart; file data starts after the
```

## Source note 3, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L33)

```text
// table, entry offsets being relative to it.
```

## Source note 4, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L34)

```text
// "XUIZ"
```

## Source note 5, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L37)

```text
// "XUIS"
```

## Source note 6, line 121

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/package.cpp#L121)

```text
// Header: magic, version (u16), total size (u32), string count (u16).
```
