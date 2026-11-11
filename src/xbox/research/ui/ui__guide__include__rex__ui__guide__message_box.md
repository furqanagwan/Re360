# Message box: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/message_box.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 9

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/message_box.h#L9)

```text
// A console message-box scene without runtime/guest dependencies. The caller
```

## Source note 2, line 10

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/message_box.h#L10)

```text
// owns the skin document until this object is destroyed and supplies rendering,
```

## Source note 3, line 11

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/message_box.h#L11)

```text
// input, sounds and actions. Creation fails if the skin lacks the required controls.
```
