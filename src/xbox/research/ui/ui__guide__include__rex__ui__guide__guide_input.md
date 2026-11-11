# Guide input: xbox ui source notes

This record preserves technical and API notes moved from `ui/guide/include/rex/ui/guide/guide_input.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 16

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L16)

```text
/// The guide chord: View (Back) and Menu (Start) held together, as Xbox
```

## Source note 2, line 17

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L17)

```text
/// backward compatibility opens the Xbox 360 guide. The Xbox button is left
```

## Source note 3, line 18

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L18)

```text
/// to Windows, which opens Game Bar with it. Fires once per press; both must
```

## Source note 4, line 19

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L19)

```text
/// be let go before it fires again.
```

## Source note 5, line 25

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L25)

```text
// a chord held when polling starts does not fire
```

## Source note 6, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L37)

```text
// left bumper
```

## Source note 7, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L38)

```text
// right bumper
```

## Source note 8, line 40

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L40)

```text
// the left stick pressed in
```

## Source note 9, line 41

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L41)

```text
// pulled past half way
```

## Source note 10, line 45

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L45)

```text
/// Pad buttons and the left stick as guide actions. Directions repeat while
```

## Source note 11, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L46)

```text
/// held (after kRepeatDelayMs, every kRepeatIntervalMs); buttons fire once
```

## Source note 12, line 47

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L47)

```text
/// per press. Buttons already held when the guide opens are ignored until
```

## Source note 13, line 48

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L48)

```text
/// released.
```

## Source note 14, line 56

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L56)

```text
/// `buttons` held at open; they will not produce actions.
```

## Source note 15, line 66

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L66)

```text
// GuideAction for the held direction, -1 when none
```

## Source note 16, line 68

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/guide/include/rex/ui/guide/guide_input.h#L68)

```text
// The triggers as two more buttons: pulled past kTriggerThreshold.
```
