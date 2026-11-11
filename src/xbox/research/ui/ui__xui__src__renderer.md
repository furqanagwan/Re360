# Renderer: xbox ui source notes

This record preserves technical and API notes moved from `ui/xui/src/renderer.cpp`.
Names and executable tokens are unchanged; licence and attribution headers
remain in the source. Historical explanations are retained as source evidence,
not newly validated hardware claims. Unanswered task notes are tracked by
[#229](https://github.com/furqanagwan/rexglue-sdk/issues/229) and its topic issues.

Part of [#230](https://github.com/furqanagwan/rexglue-sdk/issues/230).
Source pin: `fc272d5`.

## Source note 1, line 24

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L24)

```text
// Row-vector 2D affine map: x' = a x + c y + tx, y' = b x + d y + ty.
```

## Source note 2, line 29

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L29)

```text
// this after inner: (this * inner)(p) = this(inner(p)).
```

## Source note 3, line 45

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L45)

```text
// The quaternion's rotation seen from the front: its 3x3 matrix's x/y part.
```

## Source note 4, line 46

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L46)

```text
// A turn about Z rotates; a half turn about X or Y mirrors, as XUI's 3D
```

## Source note 5, line 47

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L47)

```text
// rotations look on the flat screen.
```

## Source note 6, line 65

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L65)

```text
// XUI BlendMode 1 multiplies the destination by the colour: the skin's row
```

## Source note 7, line 66

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L66)

```text
// separators (Top and Bottom, 0xffd2d5d9) darken whatever is under them by
```

## Source note 8, line 67

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L67)

```text
// the same proportion, so they look equally dark on every row. Over a normal
```

## Source note 9, line 68

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L68)

```text
// alpha blend, multiplying by a grey g is drawing black at alpha 1 - g; the
```

## Source note 10, line 69

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L69)

```text
// channels' mean stands for g (the skin's multiplied colours are near grey).
```

## Source note 11, line 101

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L101)

```text
// Subdivisions per fan triangle side for multi-stop and radial gradients.
```

## Source note 12, line 103

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L103)

```text
// Radial gradients make thin bands (the ring of light's arcs), so they are
```

## Source note 13, line 104

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L104)

```text
// subdivided by their size on screen: one step per kRadialStepPixels, within
```

## Source note 14, line 105

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L105)

```text
// these limits. A fixed 32 put some pages past 160,000 vertices.
```

## Source note 15, line 167

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L167)

```text
// The figure's outline in element units: its Bezier path scaled to the
```

## Source note 16, line 168

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L168)

```text
// element's size, or the element's box.
```

## Source note 17, line 215

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L215)

```text
// a Fill with no type is solid
```

## Source note 18, line 241

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L241)

```text
// Project the box onto the gradient direction for the 0..1 range.
```

## Source note 19, line 252

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L252)

```text
// The brush is the box's inscribed ellipse with the fill's Scale and
```

## Source note 20, line 253

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L253)

```text
// Translation (in box units, turned by its Rotation) applied to the
```

## Source note 21, line 254

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L254)

```text
// brush's texture coordinates, so the ellipse itself moves the other
```

## Source note 22, line 255

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L255)

```text
// way and grows as the scale shrinks. The ring of light's quarter
```

## Source note 23, line 256

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L256)

```text
// arcs (Scale 0.55, Translation 0.34) are centred past a corner.
```

## Source note 24, line 304

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L304)

```text
// Fans the outline from its centroid; gradients subdivide each fan triangle
```

## Source note 25, line 305

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L305)

```text
// `steps` times per side so colour follows the gradient, not just the
```

## Source note 26, line 306

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L306)

```text
// corners.
```

## Source note 27, line 326

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L326)

```text
// Row i is i/steps of the way from the centre; it has i + 1 vertices.
```

## Source note 28, line 361

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L361)

```text
// The control whose data a presenter shows: the nearest control above it.
```

## Source note 29, line 382

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L382)

```text
// Other images that are scenes are not drawn.
```

## Source note 30, line 393

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L393)

```text
// Fit, keeping the image's aspect, centred.
```

## Source note 31, line 442

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L442)

```text
// text_Label2 and a slider's Text_Slider carry a control's second label
```

## Source note 32, line 443

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L443)

```text
// (a count or a value).
```

## Source note 33, line 461

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L461)

```text
// Lay the text out in element units scaled to pixels (so glyphs are
```

## Source note 34, line 462

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L462)

```text
// rasterized at their screen size), then map the vertices through `m`.
```

## Source note 35, line 465

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L465)

```text
// Edit controls (message box bodies) wrap whatever their presenter says.
```

## Source note 36, line 472

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L472)

```text
// Drop whole UTF-8 characters.
```

## Source note 37, line 495

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L495)

```text
// Wrapped text taller than its box (a long description in a fixed pane)
```

## Source note 38, line 496

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L496)

```text
// scrolls through it as the console's did, clipped to the box.
```

## Source note 39, line 506

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L506)

```text
// About three quarters of a line a second.
```

## Source note 40, line 539

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L539)

```text
// One unwrapped line with gamerscore glyphs, each drawn as the
```

## Source note 41, line 540

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L540)

```text
// kGamerscoreImage square in the text colour, the height of a capital.
```

## Source note 42, line 559

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L559)

```text
// Each glyph between two parts, with a gap on the side that has text.
```

## Source note 43, line 587

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L587)

```text
// Beside text, centred on its digits; alone (a visual's glyph
```

## Source note 44, line 588

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L588)

```text
// presenter), centred in its box as the font places it.
```

## Source note 45, line 608

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L608)

```text
// the figure being filled has BlendMode multiply
```

## Source note 46, line 634

[Pinned source](https://github.com/furqanagwan/xbox/blob/fc272d5/ui/xui/src/renderer.cpp#L634)

```text
// Fades back in at the top, except the first time it's shown.
```
