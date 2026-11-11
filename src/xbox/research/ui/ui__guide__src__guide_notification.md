# Guide notification: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/src/guide_notification.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 20

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L20)

```text
// The popup (460x87) sits centred above the bottom of the title-safe area.
```

## Source note 2, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L24)

```text
// XamStrings: "Achievement unlocked\n%sG - %s" (with a literal \n).
```

## Source note 3, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L63)

```text
// Preferences > Notifications > Show Notifications.
```

## Source note 4, line 89

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L89)

```text
// TransTo brings it in and holds it; the playhead runs on through
```

## Source note 5, line 90

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L90)

```text
// TransFrom, which takes it away, and stops at EndTransFrom.
```

## Source note 6, line 110

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L110)

```text
// No system update: the SDK's own toast shows it.
```

## Source note 7, line 121

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L121)

```text
// still loading
```

## Source note 8, line 136

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L136)

```text
// TransTo ends in a go-to that can hold the popup on screen; the console
```

## Source note 9, line 137

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/src/guide_notification.cpp#L137)

```text
// takes it away after a few seconds with TransFrom.
```
