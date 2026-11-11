# Guide notification: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/guide_notification.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 21

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L21)

```text
/// Plays xam's notify.xur (the scr_Notification visual: the Xbox logo burst,
```

## Source note 2, line 22

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L22)

```text
/// the bar sliding out, NotifyPopup.xma) with XAM's "Achievement unlocked"
```

## Source note 3, line 23

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L23)

```text
/// text, one unlock at a time at the bottom of the screen. Without a system
```

## Source note 4, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L24)

```text
/// update it hands unlocks to `fallback`.
```

## Source note 5, line 30

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L30)

```text
// no system update: use the fallback
```

## Source note 6, line 32

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L32)

```text
/// Called on the UI thread each frame.
```

## Source note 7, line 51

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_notification.h#L51)

```text
// guarded by mutex_
```
