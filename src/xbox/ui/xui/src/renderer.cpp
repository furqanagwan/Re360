/**
 * @file        ui/xui/src/renderer.cpp
 * @brief       Draws live XUI elements onto an ImGui draw list (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/renderer.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>
#include <vector>

#include <imgui_internal.h>

#include <rex/ui/xui/runtime.h>

namespace rex::ui::xui {
namespace {

struct Affine {
  float a = 1, b = 0, c = 0, d = 1, tx = 0, ty = 0;

  ImVec2 Apply(float x, float y) const { return {a * x + c * y + tx, b * x + d * y + ty}; }

  Affine operator*(const Affine& o) const {
    return {a * o.a + c * o.b, b * o.a + d * o.b,        a * o.c + c * o.d,
            b * o.c + d * o.d, a * o.tx + c * o.ty + tx, b * o.tx + d * o.ty + ty};
  }
  float UniformScale() const { return std::sqrt(std::fabs(a * d - b * c)); }
};

Affine Translate(float x, float y) {
  return {1, 0, 0, 1, x, y};
}

Affine Scale(float x, float y) {
  return {x, 0, 0, y, 0, 0};
}

Affine Rotate(const Quat& q) {
  const float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
  const float xy = q.x * q.y, zw = q.z * q.w;
  return {1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xy - zw), 1 - 2 * (xx + zz), 0, 0};
}

struct Stop {
  float pos;
  uint32_t argb;
};

ImU32 ToImColor(uint32_t argb, float opacity) {
  const float alpha = float(argb >> 24) * std::clamp(opacity, 0.0f, 1.0f);
  return IM_COL32((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF,
                  uint32_t(std::lround(alpha)));
}

constexpr uint32_t kBlendMultiply = 1;

uint32_t MultiplyAsBlack(uint32_t argb) {
  const float grey = float(((argb >> 16) & 0xFF) + ((argb >> 8) & 0xFF) + (argb & 0xFF)) / 765.0f;
  const float alpha = float(argb >> 24) * (1.0f - grey);
  return uint32_t(std::lround(alpha)) << 24;
}

uint32_t SampleStops(const std::vector<Stop>& stops, float t) {
  if (stops.empty()) {
    return 0;
  }
  if (t <= stops.front().pos) {
    return stops.front().argb;
  }
  for (size_t i = 1; i < stops.size(); ++i) {
    if (t <= stops[i].pos) {
      const float span = stops[i].pos - stops[i - 1].pos;
      const float f = span > 0.0f ? (t - stops[i - 1].pos) / span : 1.0f;
      uint32_t out = 0;
      for (int shift = 0; shift < 32; shift += 8) {
        const float ca = float((stops[i - 1].argb >> shift) & 0xFF);
        const float cb = float((stops[i].argb >> shift) & 0xFF);
        out |= uint32_t(std::lround(ca + (cb - ca) * f)) << shift;
      }
      return out;
    }
  }
  return stops.back().argb;
}

constexpr int kGradientSteps = 10;

constexpr float kRadialStepPixels = 2.0f;
constexpr int kRadialMinSteps = 4;
constexpr int kRadialMaxSteps = 32;

template <typename T>
const T* Member(const PropertyBag* bag, std::string_view name) {
  const Value* v = bag ? bag->Find(name) : nullptr;
  return v ? v->get<T>() : nullptr;
}

class Renderer {
 public:
  Renderer(ImDrawList* list, const RenderResources& resources)
      : list_(list), resources_(resources) {}

  void Draw(const Element& element, const Affine& parent, float parent_opacity) {
    if (!element.visible()) {
      return;
    }
    const float opacity = parent_opacity * element.GetFloat("Opacity", 1.0f);
    if (opacity <= 0.0f) {
      return;
    }
    const Vec3 pos = element.position();
    const Vec3 pivot = element.GetVector("Pivot");
    const Vec3 scale = element.GetVector("Scale", Vec3{1.0f, 1.0f, 1.0f});
    const Quat rot = element.GetQuaternion("Rotation");
    const Affine local = Translate(pos.x, pos.y) * Translate(pivot.x, pivot.y) * Rotate(rot) *
                         Scale(scale.x, scale.y) * Translate(-pivot.x, -pivot.y);
    const Affine m = parent * local;
    const float w = element.width();
    const float h = element.height();

    if (element.IsA("XuiFigure")) {
      DrawFigure(element, m, w, h, opacity);
    } else if (element.IsA("XuiNineGrid")) {
      DrawNineGrid(element, m, w, h, opacity);
    } else if (element.IsA("XuiImage") || element.IsA("XuiImagePresenter")) {
      DrawImage(element, m, w, h, opacity);
    } else if (element.IsA("XuiText") || element.IsA("XuiTextPresenter")) {
      DrawText(element, m, w, h, opacity);
    }

    const bool clip = element.GetBool("ClipChildren");
    if (clip) {
      ImVec2 lo(FLT_MAX, FLT_MAX), hi(-FLT_MAX, -FLT_MAX);
      for (ImVec2 p : {m.Apply(0, 0), m.Apply(w, 0), m.Apply(w, h), m.Apply(0, h)}) {
        lo = ImVec2(std::min(lo.x, p.x), std::min(lo.y, p.y));
        hi = ImVec2(std::max(hi.x, p.x), std::max(hi.y, p.y));
      }
      list_->PushClipRect(lo, hi, true);
    }
    for (const auto& child : element.children()) {
      Draw(*child, m, opacity);
    }
    if (clip) {
      list_->PopClipRect();
    }
  }

 private:
  std::vector<ImVec2> Outline(const Element& element, float w, float h) const {
    std::vector<ImVec2> points;
    const Value* v = element.Get("Points");
    const auto* path = v ? v->get<std::shared_ptr<const Path>>() : nullptr;
    if (!path || !*path || (*path)->points.size() < 2) {
      return {{0, 0}, {w, 0}, {w, h}, {0, h}};
    }
    const Path& p = **path;
    const float sx = p.width > 0.0f ? w / p.width : 1.0f;
    const float sy = p.height > 0.0f ? h / p.height : 1.0f;
    const size_t n = p.points.size();
    for (size_t i = 0; i < n; ++i) {
      const Path::Point& a = p.points[i];
      const Path::Point& b = p.points[(i + 1) % n];
      const bool straight = a.c2x == a.x && a.c2y == a.y && b.c1x == b.x && b.c1y == b.y;
      const int steps = straight ? 1 : 8;
      for (int s = 0; s < steps; ++s) {
        const float t = float(s) / float(steps);
        const float u = 1.0f - t;
        const float x =
            u * u * u * a.x + 3 * u * u * t * a.c2x + 3 * u * t * t * b.c1x + t * t * t * b.x;
        const float y =
            u * u * u * a.y + 3 * u * u * t * a.c2y + 3 * u * t * t * b.c1y + t * t * t * b.y;
        points.push_back(ImVec2(x * sx, y * sy));
      }
    }
    return points;
  }

  float PixelsPerUnit(const Affine& m) const {
    return m.UniformScale() * resources_.pixels_per_point;
  }

  int RadialSteps(const Affine& m, float w, float h) const {
    const ImVec2 o = m.Apply(0, 0), x = m.Apply(w, 0), y = m.Apply(0, h);
    const float side =
        std::max(std::hypot(x.x - o.x, x.y - o.y), std::hypot(y.x - o.x, y.y - o.y)) *
        resources_.pixels_per_point;
    return std::clamp(int(std::ceil(side / kRadialStepPixels)), kRadialMinSteps, kRadialMaxSteps);
  }

  void DrawFigure(const Element& element, const Affine& m, float w, float h, float opacity) {
    const std::vector<ImVec2> outline = Outline(element, w, h);
    multiply_ = element.GetUnsigned("BlendMode") == kBlendMultiply;
    if (const PropertyBag* fill = element.GetCompound("Fill")) {
      const uint32_t* type = Member<uint32_t>(fill, "FillType");
      const uint32_t fill_type = type ? *type : 1;
      std::vector<Stop> stops;
      if (fill_type == 2 || fill_type == 3) {
        const auto* gradient = Member<std::shared_ptr<const PropertyBag>>(fill, "Gradient");
        const PropertyBag* g = gradient ? gradient->get() : nullptr;
        const auto* colors = Member<std::shared_ptr<const std::vector<Value>>>(g, "StopColor");
        const auto* positions = Member<std::shared_ptr<const std::vector<Value>>>(g, "StopPos");
        if (colors && *colors && positions && *positions) {
          const size_t n = std::min((*colors)->size(), (*positions)->size());
          for (size_t i = 0; i < n; ++i) {
            const Color* c = (**colors)[i].get<Color>();
            const float* p = (**positions)[i].get<float>();
            stops.push_back({p ? *p : 0.0f, c ? c->argb : 0});
          }
          std::sort(stops.begin(), stops.end(),
                    [](const Stop& x, const Stop& y) { return x.pos < y.pos; });
        }
      }
      if (fill_type == 1 || ((fill_type == 2 || fill_type == 3) && stops.empty())) {
        const Color* color = Member<Color>(fill, "FillColor");
        const uint32_t argb = color ? color->argb : 0xFF0F0F80;
        FillPolygon(m, outline, [&](ImVec2) { return argb; }, opacity, 1);
      } else if (fill_type == 2) {
        const float* rotation = Member<float>(fill, "Rotation");
        const float radians = (rotation ? *rotation : 0.0f) * 3.14159265f / 180.0f;
        const float dx = std::cos(radians), dy = std::sin(radians);

        const float extent = std::fabs(dx) * w + std::fabs(dy) * h;
        FillPolygon(
            m, outline,
            [&](ImVec2 p) {
              const float t =
                  extent > 0.0f ? ((p.x - w / 2) * dx + (p.y - h / 2) * dy) / extent + 0.5f : 0.0f;
              return SampleStops(stops, t);
            },
            opacity, stops.size() > 2 ? kGradientSteps : 1);
      } else if (fill_type == 3) {
        const Vec3* translation = Member<Vec3>(fill, "Translation");
        const Vec3* brush_scale = Member<Vec3>(fill, "Scale");
        const float* rotation = Member<float>(fill, "Rotation");
        const float radians = (rotation ? *rotation : 0.0f) * 3.14159265f / 180.0f;
        const float tx = translation ? translation->x : 0.0f;
        const float ty = translation ? translation->y : 0.0f;
        const float sx = brush_scale && brush_scale->x != 0 ? std::fabs(brush_scale->x) : 1.0f;
        const float sy = brush_scale && brush_scale->y != 0 ? std::fabs(brush_scale->y) : 1.0f;
        const float cx = (0.5f - (tx * std::cos(radians) + ty * std::sin(radians)) / sx) * w;
        const float cy = (0.5f - (-tx * std::sin(radians) + ty * std::cos(radians)) / sy) * h;
        const float rx = 0.5f * w / sx;
        const float ry = 0.5f * h / sy;
        if (stops.size() >= 2) {
          Stop& inner = stops[stops.size() - 2];
          Stop& outer = stops.back();
          if ((outer.argb >> 24) == 0 && (inner.argb & 0xFFFFFF) == (outer.argb & 0xFFFFFF)) {
            const GradientSpan fade = EdgeFadeOnScreen({inner.pos, outer.pos}, PixelsPerUnit(m));
            inner.pos = fade.start;
            outer.pos = fade.end;
          }
        }
        FillPolygon(
            m, outline,
            [&](ImVec2 p) {
              const float dx = rx > 0 ? (p.x - cx) / rx : 0.0f;
              const float dy = ry > 0 ? (p.y - cy) / ry : 0.0f;
              return SampleStops(stops, std::sqrt(dx * dx + dy * dy));
            },
            opacity, RadialSteps(m, w, h));
      }
    }
    if (const PropertyBag* stroke = element.GetCompound("Stroke")) {
      const float* width = Member<float>(stroke, "StrokeWidth");
      const Color* color = Member<Color>(stroke, "StrokeColor");
      if (width && *width > 0.0f && color) {
        std::vector<ImVec2> screen;
        for (ImVec2 p : outline) {
          screen.push_back(m.Apply(p.x, p.y));
        }
        const bool closed = element.GetBool("Closed", true);
        list_->AddPolyline(screen.data(), int(screen.size()), ToImColor(color->argb, opacity),
                           closed ? ImDrawFlags_Closed : ImDrawFlags_None,
                           *width * m.UniformScale());
      }
    }
  }

  template <typename ColorAt>
  void FillPolygon(const Affine& m, const std::vector<ImVec2>& outline, ColorAt color_at,
                   float opacity, int steps) {
    const int n = int(outline.size());
    if (n < 3) {
      return;
    }
    ImVec2 center(0, 0);
    for (ImVec2 p : outline) {
      center.x += p.x / float(n);
      center.y += p.y / float(n);
    }
    const ImVec2 uv = list_->_Data->TexUvWhitePixel;
    const int vertices_per_triangle = (steps + 1) * (steps + 2) / 2;
    for (int e = 0; e < n; ++e) {
      const ImVec2 a = outline[e];
      const ImVec2 b = outline[(e + 1) % n];
      list_->PrimReserve(steps * steps * 3, vertices_per_triangle);
      const ImDrawIdx base = ImDrawIdx(list_->_VtxCurrentIdx);

      for (int i = 0; i <= steps; ++i) {
        const float f = float(i) / float(steps);
        for (int j = 0; j <= i; ++j) {
          const float g = i ? float(j) / float(i) : 0.0f;
          const ImVec2 p(center.x + f * ((a.x - center.x) + g * (b.x - a.x)),
                         center.y + f * ((a.y - center.y) + g * (b.y - a.y)));
          const uint32_t argb = color_at(p);
          list_->PrimWriteVtx(m.Apply(p.x, p.y), uv,
                              ToImColor(multiply_ ? MultiplyAsBlack(argb) : argb, opacity));
        }
      }
      auto index = [&](int i, int j) { return ImDrawIdx(base + i * (i + 1) / 2 + j); };
      for (int i = 0; i < steps; ++i) {
        for (int j = 0; j <= i; ++j) {
          list_->PrimWriteIdx(index(i, j));
          list_->PrimWriteIdx(index(i + 1, j));
          list_->PrimWriteIdx(index(i + 1, j + 1));
          if (j < i) {
            list_->PrimWriteIdx(index(i, j));
            list_->PrimWriteIdx(index(i + 1, j + 1));
            list_->PrimWriteIdx(index(i, j + 1));
          }
        }
      }
    }
  }

  void Quad(ImTextureID texture, const Affine& m, float x0, float y0, float x1, float y1, float u0,
            float v0, float u1, float v1, ImU32 color) {
    list_->AddImageQuad(ImTextureRef(texture), m.Apply(x0, y0), m.Apply(x1, y0), m.Apply(x1, y1),
                        m.Apply(x0, y1), ImVec2(u0, v0), ImVec2(u1, v0), ImVec2(u1, v1),
                        ImVec2(u0, v1), color);
  }

  static const Element* OwningControl(const Element& presenter) {
    for (const Element* e = presenter.parent(); e; e = e->parent()) {
      if (e->IsA("XuiControl")) {
        return e;
      }
    }
    return nullptr;
  }

  void DrawImage(const Element& element, const Affine& m, float w, float h, float opacity) {
    std::string_view path = element.GetString("ImagePath");
    if (element.IsA("XuiImagePresenter")) {
      const Element* control = OwningControl(element);
      path = control ? control->GetString("ImagePath") : std::string_view();
    }
    if (!path.empty() && resources_.vector_image &&
        resources_.vector_image(
            *list_, path, [&](ImVec2 p) { return m.Apply(p.x, p.y); }, opacity)) {
      return;
    }

    if (path.empty() || path.ends_with(".xur") || !resources_.texture) {
      return;
    }
    int tw = 0, th = 0;
    ImTextureID texture = resources_.texture(path, element.package(), &tw, &th);
    if (!texture || tw <= 0 || th <= 0) {
      return;
    }
    float x0 = 0, y0 = 0, x1 = w, y1 = h;
    if (element.GetUnsigned("SizeMode") != 0) {
      const float fit = std::min(w / float(tw), h / float(th));
      const float dw = float(tw) * fit, dh = float(th) * fit;
      x0 = (w - dw) / 2;
      y0 = (h - dh) / 2;
      x1 = x0 + dw;
      y1 = y0 + dh;
    }
    Quad(texture, m, x0, y0, x1, y1, 0, 0, 1, 1, ToImColor(0xFFFFFFFF, opacity));
  }

  void DrawNineGrid(const Element& element, const Affine& m, float w, float h, float opacity) {
    const std::string_view path = element.GetString("TextureFileName");
    if (path.empty() || !resources_.texture) {
      return;
    }
    int tw = 0, th = 0;
    ImTextureID texture = resources_.texture(path, element.package(), &tw, &th);
    if (!texture || tw <= 0 || th <= 0) {
      return;
    }
    const float l = float(element.GetUnsigned("LeftOffset"));
    const float t = float(element.GetUnsigned("TopOffset"));
    const float r = float(element.GetUnsigned("RightOffset"));
    const float b = float(element.GetUnsigned("BottomOffset"));
    const float xs[4] = {0, std::min(l, w), std::max(w - r, std::min(l, w)), w};
    const float ys[4] = {0, std::min(t, h), std::max(h - b, std::min(t, h)), h};
    const float us[4] = {0, l / tw, 1 - r / tw, 1};
    const float vs[4] = {0, t / th, 1 - b / th, 1};
    const bool no_center = element.GetBool("NoCenter");
    const ImU32 color = ToImColor(0xFFFFFFFF, opacity);
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
        if (no_center && i == 1 && j == 1) {
          continue;
        }
        if (xs[i + 1] > xs[i] && ys[j + 1] > ys[j]) {
          Quad(texture, m, xs[i], ys[j], xs[i + 1], ys[j + 1], us[i], vs[j], us[i + 1], vs[j + 1],
               color);
        }
      }
    }
  }

  void DrawText(const Element& element, const Affine& m, float w, float h, float opacity) {
    std::string text;
    if (element.IsA("XuiText")) {
      text = element.text();
    } else if (const Element* control = OwningControl(element)) {
      text = element.id() == "text_Label2" || element.id() == "Text_Slider"
                 ? control->secondary_text()
                 : std::string(control->text());
    }
    if (text.empty()) {
      return;
    }
    const uint32_t style = element.GetUnsigned("TextStyle");
    ImFont* font = (style & kTextBold) && resources_.bold_font ? resources_.bold_font
                                                               : resources_.regular_font;
    if (!font) {
      return;
    }
    const float s = m.UniformScale();
    if (s <= 0.0f) {
      return;
    }

    const float size = element.GetFloat("PointSize", 14.0f) * kPointToSceneUnits * s;
    const float box_w = w * s, box_h = h * s;

    const Element* owner = element.IsA("XuiTextPresenter") ? OwningControl(element) : nullptr;
    const bool wrap = !(style & kTextNoWrap) || (owner && owner->IsA("XuiEdit"));
    if (!wrap && (style & kTextEllipsis)) {
      if (font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str()).x > box_w) {
        while (!text.empty() &&
               font->CalcTextSizeA(size, FLT_MAX, 0.0f, (text + "...").c_str()).x > box_w) {
          do {
            text.pop_back();
          } while (!text.empty() && (uint8_t(text.back()) & 0xC0) == 0x80);
        }
        text += "...";
      }
    }
    if (text.find(kGamerscoreGlyph) != std::string::npos && resources_.texture) {
      DrawGlyphLine(text, *font, size, style, box_w, box_h, element, m, s, opacity);
      return;
    }
    const float wrap_width = wrap ? box_w : 0.0f;
    const ImVec2 extent = font->CalcTextSizeA(size, FLT_MAX, wrap_width, text.c_str());
    float x = 0.0f, y = 0.0f;
    if (style & kTextRight) {
      x = box_w - extent.x;
    } else if (style & kTextCenter) {
      x = (box_w - extent.x) / 2;
    }
    if (style & kTextVerticalCenter) {
      y = (box_h - extent.y) / 2;
    }

    bool clipped = false;
    if (wrap && !(owner && owner->IsA("XuiEdit")) && box_h >= size && extent.y > box_h &&
        resources_.text_scroll) {
      TextScroll& scroll = (*resources_.text_scroll)[&element];
      const double now = ImGui::GetTime();
      if (scroll.text != text) {
        scroll.text = text;
        scroll.start = now;
      }

      const TextScrollFrame frame =
          TextScrollAt(now - scroll.start, extent.y - box_h, size * 0.75f);
      y -= frame.offset;
      opacity *= frame.alpha;
      ImVec2 lo(FLT_MAX, FLT_MAX), hi(-FLT_MAX, -FLT_MAX);
      for (ImVec2 p : {m.Apply(0, 0), m.Apply(w, 0), m.Apply(w, h), m.Apply(0, h)}) {
        lo = ImVec2(std::min(lo.x, p.x), std::min(lo.y, p.y));
        hi = ImVec2(std::max(hi.x, p.x), std::max(hi.y, p.y));
      }
      list_->PushClipRect(lo, hi, true);
      clipped = true;
    }
    const ImVec4 no_cull(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
    const uint32_t shadow = element.GetColor("DropShadowColor");
    const uint32_t color = element.GetColor("TextColor", 0xFFFFFFFF);
    const int first_vertex = list_->VtxBuffer.Size;
    if (shadow >> 24) {
      font->RenderText(list_, size, ImVec2(x + s, y + s), ToImColor(shadow, opacity), no_cull,
                       text.c_str(), nullptr, wrap_width);
    }
    font->RenderText(list_, size, ImVec2(x, y), ToImColor(color, opacity), no_cull, text.c_str(),
                     nullptr, wrap_width);
    const Affine to_screen = m * Scale(1.0f / s, 1.0f / s);
    for (int i = first_vertex; i < list_->VtxBuffer.Size; ++i) {
      ImDrawVert& v = list_->VtxBuffer[i];
      v.pos = to_screen.Apply(v.pos.x, v.pos.y);
    }
    if (clipped) {
      list_->PopClipRect();
    }
  }

  void DrawGlyphLine(const std::string& text, ImFont& font, float size, uint32_t style, float box_w,
                     float box_h, const Element& element, const Affine& m, float s, float opacity) {
    int glyph_w = 0, glyph_h = 0;
    ImTextureID glyph = resources_.texture(kGamerscoreImage, "", &glyph_w, &glyph_h);
    const float glyph_size = size * 0.72f;
    const float gap = size * 0.12f;
    std::vector<std::string_view> parts;
    for (size_t start = 0;;) {
      const size_t at = text.find(kGamerscoreGlyph, start);
      parts.push_back(std::string_view(text).substr(start, at - start));
      if (at == std::string::npos) {
        break;
      }
      start = at + kGamerscoreGlyph.size();
    }
    auto width_of = [&](std::string_view part) {
      return font.CalcTextSizeA(size, FLT_MAX, 0.0f, part.data(), part.data() + part.size()).x;
    };

    float total = 0.0f;
    for (size_t i = 0; i < parts.size(); ++i) {
      total += width_of(parts[i]);
      if (i + 1 < parts.size()) {
        total += glyph_size + (parts[i].empty() ? 0.0f : gap) + (parts[i + 1].empty() ? 0.0f : gap);
      }
    }
    float x = 0.0f, y = 0.0f;
    if (style & kTextRight) {
      x = box_w - total;
    } else if (style & kTextCenter) {
      x = (box_w - total) / 2;
    }
    if (style & kTextVerticalCenter) {
      y = (box_h - size) / 2;
    }
    const ImVec4 no_cull(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
    const uint32_t color = ToImColor(element.GetColor("TextColor", 0xFFFFFFFF), opacity);
    const int first_vertex = list_->VtxBuffer.Size;
    for (size_t i = 0; i < parts.size(); ++i) {
      const std::string_view part = parts[i];
      if (!part.empty()) {
        font.RenderText(list_, size, ImVec2(x, y), color, no_cull, part.data(),
                        part.data() + part.size(), 0.0f);
        x += width_of(part) + gap;
      }
      if (i + 1 < parts.size()) {
        const bool alone = text.size() == kGamerscoreGlyph.size();
        const float top =
            alone ? (box_h - glyph_size) / 2 + size * 0.1f : y + size * 0.52f - glyph_size / 2;
        if (glyph && glyph_w > 0 && glyph_h > 0) {
          list_->AddImage(glyph, ImVec2(x, top), ImVec2(x + glyph_size, top + glyph_size),
                          ImVec2(0, 0), ImVec2(1, 1), color);
        }
        x += glyph_size + (parts[i + 1].empty() ? 0.0f : gap);
      }
    }
    const Affine to_screen = m * Scale(1.0f / s, 1.0f / s);
    for (int i = first_vertex; i < list_->VtxBuffer.Size; ++i) {
      ImDrawVert& v = list_->VtxBuffer[i];
      v.pos = to_screen.Apply(v.pos.x, v.pos.y);
    }
  }

  ImDrawList* list_;
  const RenderResources& resources_;
  bool multiply_ = false;
};

}

GradientSpan EdgeFadeOnScreen(GradientSpan fade, float pixels_per_unit) {
  if (pixels_per_unit <= 1.0f) {
    return fade;
  }
  const float middle = (fade.start + fade.end) / 2;
  const float half = (fade.end - fade.start) / (2 * pixels_per_unit);
  return {middle - half, middle + half};
}

TextScrollFrame TextScrollAt(double elapsed, float overflow, float speed) {
  constexpr double kHoldTop = 2.5, kHoldBottom = 2.0, kFade = 0.6;
  if (overflow <= 0 || speed <= 0) {
    return {};
  }
  const double scroll = overflow / speed;
  const double cycle = kHoldTop + scroll + kHoldBottom + kFade;
  const double since = std::max(0.0, elapsed);
  const double round = std::floor(since / cycle);
  const double t = since - round * cycle;
  TextScrollFrame frame;
  if (t < kHoldTop) {
    frame.alpha = round > 0 ? float(std::min(1.0, t / kFade)) : 1.0f;
  } else if (t < kHoldTop + scroll) {
    frame.offset = float((t - kHoldTop) * speed);
  } else {
    frame.offset = overflow;
    const double fade = t - (kHoldTop + scroll + kHoldBottom);
    if (fade > 0) {
      frame.alpha = float(std::max(0.0, 1.0 - fade / kFade));
    }
  }
  return frame;
}

void Render(ImDrawList* list, const Element& root, ImVec2 origin, float scale, float opacity,
            const RenderResources& resources) {
  Renderer renderer(list, resources);
  renderer.Draw(root, Translate(origin.x, origin.y) * Scale(scale, scale), opacity);
}

}
