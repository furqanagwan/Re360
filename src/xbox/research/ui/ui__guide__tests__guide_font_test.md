# Guide font test: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/tests/guide_font_test.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 15

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L15)

```text
// Without a guide bundle in the test, the guide falls back to Segoe UI, which
```

## Source note 2, line 16

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L16)

```text
// has these scripts. The drawer builds its atlas once (ImGui's legacy mode,
```

## Source note 3, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L17)

```text
// GetTexDataAsRGBA32), so only glyphs in the guide's ranges exist.
```

## Source note 4, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L32)

```text
// e acute, Latin-1
```

## Source note 5, line 33

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L33)

```text
// S comma below, Latin Extended-B
```

## Source note 6, line 35

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L35)

```text
// Cyrillic Zhe
```

## Source note 7, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/tests/guide_font_test.cpp#L36)

```text
// Outside the ranges (Armenian), even though Segoe UI has it.
```
