# Dlc catalog: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/dlc_catalog.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 27

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L27)

```text
// marketplace media ID, a GUID ("D4C83E1F-243B-...")
```

## Source note 2, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L32)

```text
// yyyy-mm-dd, or empty
```

## Source note 3, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L33)

```text
// 420 x 95 PNG, or empty
```

## Source note 4, line 34

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L34)

```text
// 64 x 64 PNG, or empty
```

## Source note 5, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L37)

```text
/// "RXDLC001", a count, then each entry's strings and images, every field a
```

## Source note 6, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L38)

```text
/// little-endian u32 length and its bytes.
```

## Source note 7, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L42)

```text
/// Called by the catalogue rexglue_configure_target builds into the title.
```

## Source note 8, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L44)

```text
/// The built-in catalogue, empty when the title has none.
```

## Source note 9, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L46)

```text
/// The built-in catalogue read once; empty when there is none or it is bad.
```

## Source note 10, line 48

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/dlc_catalog.h#L48)

```text
/// The built-in entry with this media ID, or null.
```
