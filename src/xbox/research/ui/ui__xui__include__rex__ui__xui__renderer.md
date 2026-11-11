# Renderer: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/include/rex/ui/xui/renderer.h`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 26

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L26)

```text
// Text style bits, read from the skin's named label visuals (docs/xbox-guide.md).
```

## Source note 2, line 36

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L36)

```text
/// XUI point sizes to scene units. Measured against a 1080p capture of the
```

## Source note 3, line 37

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L37)

```text
/// dashboard 2.0.17559 guide, where "Xbox Home" stands 24 of its row's 61
```

## Source note 4, line 38

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L38)

```text
/// pixels (RG-GDK-043); the earlier 1.2, from the scenes' proportions alone,
```

## Source note 5, line 39

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L39)

```text
/// drew text a quarter too small.
```

## Source note 6, line 42

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L42)

```text
/// The Segoe Xbox gamerscore glyph, private use U+E00A, in UTF-8 (the skin's
```

## Source note 7, line 43

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L43)

```text
/// btn_Count_achiev glyph_presenter). The console's XUI fonts are encrypted,
```

## Source note 8, line 44

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L44)

```text
/// so text drawing puts the image `kGamerscoreImage` (a white disc with a G
```

## Source note 9, line 45

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L45)

```text
/// cut out, from sharedres GScore_white.png) in its place.
```

## Source note 10, line 50

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L50)

```text
/// Texture for a scene image path, resolved against `package`. Returns an
```

## Source note 11, line 51

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L51)

```text
/// empty ImTextureRef when missing and sets the image's pixel size.
```

## Source note 12, line 57

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L57)

```text
/// Draws an image path as vectors instead of its texture, so it stays sharp
```

## Source note 13, line 58

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L58)

```text
/// at any resolution. `to_screen` maps the image element's own units.
```

## Source note 14, line 59

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L59)

```text
/// Returns false to draw the path as usual.
```

## Source note 15, line 63

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L63)

```text
/// When each overflowing text began scrolling (TextScrollOffset), kept
```

## Source note 16, line 64

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L64)

```text
/// across frames; null turns the scrolling off.
```

## Source note 17, line 67

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L67)

```text
/// Physical pixels per draw list unit: the window's DPI scale when the host
```

## Source note 18, line 68

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L68)

```text
/// lays ImGui out in logical units (3 for 4K at 300% scaling). Gradients
```

## Source note 19, line 69

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L69)

```text
/// are subdivided and soft edges sized for the physical pixels.
```

## Source note 20, line 73

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L73)

```text
/// The span of a gradient's last stops, as positions along the gradient.
```

## Source note 21, line 78

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L78)

```text
/// A radial gradient that ends by fading its colour out is the skin's soft
```

## Source note 22, line 79

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L79)

```text
/// edge for a disc (the radio buttons' discs and dot): about 1.5 pixels at the
```

## Source note 23, line 80

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L80)

```text
/// console's 720p. Drawn `pixels_per_unit` larger, the fade keeps that width
```

## Source note 24, line 81

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L81)

```text
/// on screen, around the same middle, instead of growing into a blur.
```

## Source note 25, line 84

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/include/rex/ui/xui/renderer.h#L84)

```text
/// Draws `root` with scene unit (x, y) at screen (origin + scale * (x, y)).
```
