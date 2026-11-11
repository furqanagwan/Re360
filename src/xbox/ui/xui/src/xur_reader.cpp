/**
 * @file        ui/xui/src/xur_reader.cpp
 * @brief       XUR v8 scene reader (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/document.h>

#include <algorithm>
#include <bit>
#include <map>

#include <fmt/format.h>

namespace rex::ui::xui {

const Value* PropertyBag::Find(std::string_view name) const {
  for (const Entry& entry : entries) {
    if (entry.def && entry.def->name == name) {
      return &entry.value;
    }
  }
  return nullptr;
}

std::string_view Node::id() const {
  const Value* value = Find("Id");
  const std::string* id = value ? value->get<std::string>() : nullptr;
  return id ? std::string_view(*id) : std::string_view();
}

const Node* Node::FindById(std::string_view wanted) const {
  for (const Node& child : children) {
    if (child.id() == wanted) {
      return &child;
    }
  }
  for (const Node& child : children) {
    if (const Node* found = child.FindById(wanted)) {
      return found;
    }
  }
  return nullptr;
}

namespace {

constexpr uint32_t kMagic = 0x58554942;
constexpr uint32_t kVersion = 8;
constexpr int kMaxDepth = 64;

constexpr uint32_t FourCC(const char (&s)[5]) {
  return uint32_t(uint8_t(s[0])) << 24 | uint32_t(uint8_t(s[1])) << 16 |
         uint32_t(uint8_t(s[2])) << 8 | uint32_t(uint8_t(s[3]));
}

class ByteReader {
 public:
  explicit ByteReader(std::span<const uint8_t> bytes) : bytes_(bytes) {}

  bool ok() const { return ok_; }
  bool at_end() const { return pos_ == bytes_.size(); }
  size_t pos() const { return pos_; }
  void Seek(size_t pos) {
    if (pos > bytes_.size()) {
      ok_ = false;
      return;
    }
    pos_ = pos;
  }

  uint8_t U8() {
    if (!Need(1)) {
      return 0;
    }
    return bytes_[pos_++];
  }
  uint16_t U16() {
    if (!Need(2)) {
      return 0;
    }
    uint16_t v = uint16_t(bytes_[pos_] << 8 | bytes_[pos_ + 1]);
    pos_ += 2;
    return v;
  }
  uint32_t U32() {
    if (!Need(4)) {
      return 0;
    }
    const uint8_t* p = bytes_.data() + pos_;
    pos_ += 4;
    return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
  }
  float F32() { return std::bit_cast<float>(U32()); }
  uint32_t Packed() {
    uint8_t first = U8();
    if (first == 0xFF) {
      return U32();
    }
    if (first < 0xF0) {
      return first;
    }
    return uint32_t(first & 0x0F) << 8 | U8();
  }

 private:
  bool Need(size_t n) {
    if (!ok_ || bytes_.size() - pos_ < n) {
      ok_ = false;
      return false;
    }
    return true;
  }

  std::span<const uint8_t> bytes_;
  size_t pos_ = 0;
  bool ok_ = true;
};

struct KeyframeRecord {
  int32_t frame;
  Interpolation interpolation;
  int8_t ease_in, ease_out;
  uint8_t ease_scale;
  uint32_t value_index;
};

class XurReader {
 public:
  std::optional<Document> Read(std::span<const uint8_t> bytes, std::string* error);

 private:
  bool Fail(std::string message) {
    if (error_.empty()) {
      error_ = std::move(message);
    }
    return false;
  }

  bool ReadSections(std::span<const uint8_t> bytes);
  bool ReadNode(ByteReader& r, Node& node, int depth);
  bool ReadProperties(ByteReader& r, const ClassDef* cls, PropertyBag& bag);
  bool ReadClassProperties(ByteReader& r, const ClassDef* cls, PropertyBag& bag,
                           uint32_t& value_count);
  bool ReadValue(ByteReader& r, const PropDef& def, Value& value);
  bool ReadSingleValue(ByteReader& r, const PropDef& def, Value& value);
  bool ReadTimeline(ByteReader& r, const Node& owner, Timeline& timeline);
  bool PoolValue(PropType type, uint32_t index, Value& value);
  std::shared_ptr<const Path> ReadPath(uint32_t offset);

  std::string error_;
  std::vector<std::string> strings_;
  std::vector<Vec3> vectors_;
  std::vector<Quat> quaternions_;
  std::vector<float> floats_;
  std::vector<Color> colors_;
  std::span<const uint8_t> custom_;
  std::vector<uint32_t> keyframe_values_;
  std::vector<KeyframeRecord> keyframes_;
  std::vector<NamedFrame> named_frames_;
  std::span<const uint8_t> data_;
  std::vector<std::shared_ptr<const PropertyBag>> shared_bags_;
  std::vector<std::shared_ptr<const PropertyBag>> compounds_;
  std::map<uint32_t, std::shared_ptr<const Path>> paths_;
  uint32_t node_count_ = 0;
};

std::optional<Document> XurReader::Read(std::span<const uint8_t> bytes, std::string* error) {
  auto finish = [&]() -> std::optional<Document> {
    if (error) {
      *error = error_;
    }
    return std::nullopt;
  };
  ByteReader r(bytes);
  if (r.U32() != kMagic) {
    Fail("not a XUR file");
    return finish();
  }
  if (uint32_t version = r.U32(); version != kVersion) {
    Fail(fmt::format("XUR version {} is not supported (only 8)", version));
    return finish();
  }
  r.U32();
  r.U16();
  const uint32_t file_size = r.U32();
  const uint16_t section_count = r.U16();
  uint32_t counts[12];
  for (uint32_t& count : counts) {
    count = r.Packed();
  }
  if (!r.ok() || file_size > bytes.size()) {
    Fail("XUR header is truncated");
    return finish();
  }

  struct SectionEntry {
    uint32_t magic, offset, length;
  };
  std::vector<SectionEntry> sections(section_count);
  for (SectionEntry& s : sections) {
    s.magic = r.U32();
    s.offset = r.U32();
    s.length = r.U32();
    if (uint64_t(s.offset) + s.length > file_size) {
      Fail("XUR section runs past the end");
      return finish();
    }
  }
  if (!r.ok()) {
    Fail("XUR section table is truncated");
    return finish();
  }
  auto section = [&](uint32_t magic) -> std::span<const uint8_t> {
    for (const SectionEntry& s : sections) {
      if (s.magic == magic) {
        return bytes.subspan(s.offset, s.length);
      }
    }
    return {};
  };

  {
    strings_.push_back("");
    std::span<const uint8_t> strn = section(FourCC("STRN"));
    if (!strn.empty()) {
      ByteReader s(strn);
      s.U32();
      uint16_t count = s.U16();
      for (uint16_t i = 0; i < count && s.ok(); ++i) {
        std::string str;
        for (uint8_t c = s.U8(); c != 0 && s.ok(); c = s.U8()) {
          str.push_back(char(c));
        }
        strings_.push_back(std::move(str));
      }
      if (!s.ok()) {
        Fail("XUR STRN section is truncated");
        return finish();
      }
    }
  }
  for (ByteReader s(section(FourCC("VECT"))); !s.at_end() && s.ok();) {
    Vec3 v;
    v.x = s.F32();
    v.y = s.F32();
    v.z = s.F32();
    vectors_.push_back(v);
  }
  for (ByteReader s(section(FourCC("QUAT"))); !s.at_end() && s.ok();) {
    Quat q;
    q.x = s.F32();
    q.y = s.F32();
    q.z = s.F32();
    q.w = s.F32();
    quaternions_.push_back(q);
  }
  for (ByteReader s(section(FourCC("FLOT"))); !s.at_end() && s.ok();) {
    floats_.push_back(s.F32());
  }
  for (ByteReader s(section(FourCC("COLR"))); !s.at_end() && s.ok();) {
    colors_.push_back(Color{s.U32()});
  }
  custom_ = section(FourCC("CUST"));
  for (ByteReader s(section(FourCC("KEYP"))); !s.at_end() && s.ok();) {
    keyframe_values_.push_back(s.Packed());
  }
  {
    ByteReader s(section(FourCC("KEYD")));
    while (!s.at_end() && s.ok()) {
      KeyframeRecord k{};
      k.frame = int32_t(s.Packed());
      const uint8_t flags = s.U8() & 0x3F;
      k.interpolation = Interpolation::kLinear;
      switch (flags) {
        case 0x0:
        case 0x3:
          break;
        case 0x1:
          k.interpolation = Interpolation::kNone;
          break;
        case 0x2:
          k.interpolation = Interpolation::kEase;
          k.ease_in = int8_t(s.U8());
          k.ease_out = int8_t(s.U8());
          k.ease_scale = s.U8();
          break;
        case 0xA:
          s.Packed();
          break;
        case 0xB:
          s.U8();
          break;
        default:
          Fail(fmt::format("XUR keyframe flags {:#x} are not known", flags));
          return finish();
      }
      k.value_index = s.Packed();
      keyframes_.push_back(k);
    }
    if (!s.ok()) {
      Fail("XUR KEYD section is truncated");
      return finish();
    }
  }
  {
    ByteReader s(section(FourCC("NAME")));
    while (!s.at_end() && s.ok()) {
      NamedFrame frame;
      const uint32_t name = s.Packed();
      frame.frame = int32_t(s.Packed());
      const uint8_t command = s.U8();
      if (command > uint8_t(FrameCommand::kGoToAndStop)) {
        Fail(fmt::format("XUR named frame command {} is not known", command));
        return finish();
      }
      frame.command = FrameCommand(command);
      uint32_t target = 0;
      if (frame.command == FrameCommand::kGoTo || frame.command == FrameCommand::kGoToAndPlay ||
          frame.command == FrameCommand::kGoToAndStop) {
        target = s.Packed();
      }
      if (name >= strings_.size() || target >= strings_.size()) {
        Fail("XUR named frame names a missing string");
        return finish();
      }
      frame.name = strings_[name];
      frame.target = strings_[target];
      named_frames_.push_back(std::move(frame));
    }
    if (!s.ok()) {
      Fail("XUR NAME section is truncated");
      return finish();
    }
  }

  data_ = section(FourCC("DATA"));
  if (data_.empty()) {
    Fail("XUR has no DATA section");
    return finish();
  }
  Document document;
  ByteReader d(data_);
  if (!ReadNode(d, document.root, 0)) {
    return finish();
  }
  if (!d.at_end()) {
    Fail(fmt::format("XUR DATA has {} unread bytes", data_.size() - d.pos()));
    return finish();
  }

  if (node_count_ != counts[0]) {
    Fail(fmt::format("XUR declares {} objects but {} were read", counts[0], node_count_));
    return finish();
  }
  return document;
}

bool XurReader::ReadNode(ByteReader& r, Node& node, int depth) {
  if (depth > kMaxDepth) {
    return Fail("XUR element tree is too deep");
  }
  ++node_count_;
  const uint32_t class_index = r.Packed();
  const uint8_t flags = r.U8();
  if (!r.ok() || class_index >= strings_.size()) {
    return Fail("XUR element is truncated");
  }
  node.class_name = strings_[class_index];
  node.cls = FindClass(node.class_name);
  if (!node.cls) {
    return Fail(fmt::format("XUI class {} is not in the schema", node.class_name));
  }
  if (flags & 0x1) {
    auto bag = std::make_shared<PropertyBag>();
    if (!ReadProperties(r, node.cls, *bag)) {
      return false;
    }
    node.props = bag;
    shared_bags_.push_back(std::move(bag));
  } else if (flags & 0x8) {
    const uint32_t index = r.Packed();
    if (index >= shared_bags_.size()) {
      return Fail("XUR element shares missing properties");
    }
    node.props = shared_bags_[index];
  }
  if (flags & 0x2) {
    const uint32_t count = r.Packed();
    if (!r.ok() || count > data_.size()) {
      return Fail("XUR child count is not valid");
    }
    node.children.resize(count);
    for (Node& child : node.children) {
      if (!ReadNode(r, child, depth + 1)) {
        return false;
      }
    }
  }
  if (flags & 0x4) {
    const uint32_t frame_count = r.Packed();
    if (frame_count) {
      const uint32_t base = r.Packed();
      if (uint64_t(base) + frame_count > named_frames_.size()) {
        return Fail("XUR element names missing named frames");
      }
      node.named_frames.assign(named_frames_.begin() + base,
                               named_frames_.begin() + base + frame_count);
    }

    if (!node.children.empty()) {
      const uint32_t timeline_count = r.Packed();
      if (!r.ok() || timeline_count > data_.size()) {
        return Fail("XUR timeline count is not valid");
      }
      node.timelines.resize(timeline_count);
      for (Timeline& timeline : node.timelines) {
        if (!ReadTimeline(r, node, timeline)) {
          return false;
        }
      }
    }
  }
  return r.ok() || Fail("XUR element is truncated");
}

bool XurReader::ReadProperties(ByteReader& r, const ClassDef* cls, PropertyBag& bag) {
  const uint32_t declared = r.Packed();
  uint32_t value_count = 0;
  for (const ClassDef* level : ClassChain(cls)) {
    if (!ReadClassProperties(r, level, bag, value_count)) {
      return false;
    }
  }
  if (value_count != declared) {
    return Fail(fmt::format("XUI class {} declares {} property values but {} were read", cls->name,
                            declared, value_count));
  }
  return true;
}

bool XurReader::ReadClassProperties(ByteReader& r, const ClassDef* cls, PropertyBag& bag,
                                    uint32_t& value_count) {
  uint32_t mask = r.Packed();
  for (size_t i = 0; i < cls->props.size() && i < 32; ++i) {
    if (!(mask & (1u << i))) {
      continue;
    }
    PropertyBag::Entry entry;
    entry.def = &cls->props[i];
    if (!ReadValue(r, *entry.def, entry.value)) {
      return false;
    }
    const auto* list = entry.value.get<std::shared_ptr<const std::vector<Value>>>();
    value_count += list && *list ? uint32_t((*list)->size()) : 1;
    bag.entries.push_back(std::move(entry));
  }

  const uint32_t known = uint32_t(std::min<size_t>(cls->props.size(), 32));
  mask = known >= 32 ? 0 : mask >> known;
  for (; mask; mask >>= 1) {
    if (mask & 1) {
      PropertyBag::Entry entry;
      entry.value.data = r.Packed();
      bag.entries.push_back(std::move(entry));
      ++value_count;
    }
  }
  return r.ok() || Fail("XUR properties are truncated");
}

bool XurReader::ReadValue(ByteReader& r, const PropDef& def, Value& value) {
  if (!def.indexed) {
    return ReadSingleValue(r, def, value);
  }
  const uint8_t count = r.U8();
  auto list = std::make_shared<std::vector<Value>>(count);
  for (Value& element : *list) {
    if (!ReadSingleValue(r, def, element)) {
      return false;
    }
  }
  value.data = std::shared_ptr<const std::vector<Value>>(std::move(list));
  return true;
}

bool XurReader::ReadSingleValue(ByteReader& r, const PropDef& def, Value& value) {
  switch (def.type) {
    case PropType::kBool:
      value.data = r.U8() != 0;
      break;
    case PropType::kInteger:
      value.data = int32_t(r.Packed());
      break;
    case PropType::kUnsigned:
      value.data = r.Packed();
      break;
    case PropType::kString:
    case PropType::kFloat:
    case PropType::kVector:
    case PropType::kQuaternion:
    case PropType::kColor:
      if (!PoolValue(def.type, r.Packed(), value)) {
        return Fail(fmt::format("XUR property {} refers to a missing value", def.name));
      }
      break;
    case PropType::kCustom: {
      auto path = ReadPath(r.Packed());
      if (!path) {
        return Fail(fmt::format("XUR property {} refers to a missing figure", def.name));
      }
      value.data = std::move(path);
      break;
    }
    case PropType::kObject: {
      const uint32_t index = r.Packed();
      if (index < compounds_.size()) {
        value.data = compounds_[index];
        break;
      }
      const ClassDef* cls = CompoundClass(def.name);
      if (!cls) {
        return Fail(fmt::format("XUR compound property {} is not known", def.name));
      }
      auto bag = std::make_shared<PropertyBag>();
      const uint32_t declared = r.Packed();
      uint32_t value_count = 0;
      if (!ReadClassProperties(r, cls, *bag, value_count)) {
        return false;
      }
      if (value_count != declared) {
        return Fail(fmt::format("XUR compound {} declares {} values but {} were read", def.name,
                                declared, value_count));
      }
      compounds_.push_back(bag);
      value.data = std::shared_ptr<const PropertyBag>(std::move(bag));
      break;
    }
  }
  return r.ok() || Fail("XUR property value is truncated");
}

bool XurReader::PoolValue(PropType type, uint32_t index, Value& value) {
  switch (type) {
    case PropType::kString:
      if (index >= strings_.size()) {
        return false;
      }
      value.data = strings_[index];
      return true;
    case PropType::kFloat:
      if (index >= floats_.size()) {
        return false;
      }
      value.data = floats_[index];
      return true;
    case PropType::kVector:
      if (index >= vectors_.size()) {
        return false;
      }
      value.data = vectors_[index];
      return true;
    case PropType::kQuaternion:
      if (index >= quaternions_.size()) {
        return false;
      }
      value.data = quaternions_[index];
      return true;
    case PropType::kColor:
      if (index >= colors_.size()) {
        return false;
      }
      value.data = colors_[index];
      return true;
    case PropType::kBool:
      value.data = index != 0;
      return true;
    case PropType::kInteger:
      value.data = int32_t(index);
      return true;
    case PropType::kUnsigned:
      value.data = index;
      return true;
    case PropType::kObject:
    case PropType::kCustom:
      return false;
  }
  return false;
}

std::shared_ptr<const Path> XurReader::ReadPath(uint32_t offset) {
  if (auto it = paths_.find(offset); it != paths_.end()) {
    return it->second;
  }

  ByteReader r(custom_);
  r.Seek(offset);
  r.U32();
  auto path = std::make_shared<Path>();
  path->width = r.F32();
  path->height = r.F32();
  const uint32_t count = r.U32();
  if (!r.ok() || count > custom_.size() / 24) {
    return nullptr;
  }
  path->points.resize(count);
  for (Path::Point& p : path->points) {
    p.x = r.F32();
    p.y = r.F32();
    p.c1x = r.F32();
    p.c1y = r.F32();
    p.c2x = r.F32();
    p.c2y = r.F32();
  }
  if (!r.ok()) {
    return nullptr;
  }
  paths_[offset] = path;
  return path;
}

bool XurReader::ReadTimeline(ByteReader& r, const Node& owner, Timeline& timeline) {
  const uint32_t id = r.Packed();
  if (!r.ok() || id >= strings_.size()) {
    return Fail("XUR timeline names a missing element");
  }
  timeline.target_id = strings_[id];

  std::vector<const ClassDef*> chain;
  if (const Node* target = owner.FindById(timeline.target_id)) {
    chain = ClassChain(target->cls);
    std::reverse(chain.begin(), chain.end());
  }
  const uint32_t prop_count = r.Packed();
  if (!r.ok() || prop_count > data_.size()) {
    return Fail("XUR timeline property count is not valid");
  }
  timeline.props.resize(prop_count);
  for (AnimatedProperty& prop : timeline.props) {
    const uint8_t packed = r.U8();
    const uint8_t depth = packed & 0x7F;
    const bool indexed = (packed & 0x80) != 0;
    const uint8_t class_index = r.U8();
    const ClassDef* cls = class_index < chain.size() ? chain[class_index] : nullptr;
    for (uint8_t level = 0; level < depth; ++level) {
      const uint8_t prop_index = r.U8();
      if (!cls || prop_index >= cls->props.size()) {
        cls = nullptr;
        continue;
      }
      const PropDef* def = &cls->props[prop_index];
      prop.path.push_back(def);
      cls = level + 1 < depth ? CompoundClass(def->name) : cls;
    }
    if (!cls) {
      prop.path.clear();
    }
    if (indexed) {
      prop.index = int32_t(r.Packed());
    }
  }
  const uint32_t keyframe_count = r.Packed();
  const uint32_t keyframe_base = r.Packed();
  if (!r.ok() || uint64_t(keyframe_base) + keyframe_count > keyframes_.size()) {
    return Fail("XUR timeline names missing keyframes");
  }
  timeline.keyframes.resize(keyframe_count);
  for (uint32_t k = 0; k < keyframe_count; ++k) {
    const KeyframeRecord& record = keyframes_[keyframe_base + k];
    Keyframe& keyframe = timeline.keyframes[k];
    keyframe.frame = record.frame;
    keyframe.interpolation = record.interpolation;
    keyframe.ease_in = record.ease_in;
    keyframe.ease_out = record.ease_out;
    keyframe.ease_scale = record.ease_scale;
    keyframe.values.resize(prop_count);
    for (uint32_t p = 0; p < prop_count; ++p) {
      const uint64_t slot = uint64_t(record.value_index) + p;
      if (slot >= keyframe_values_.size()) {
        return Fail("XUR keyframe names a missing value");
      }
      const uint32_t raw = keyframe_values_[size_t(slot)];
      Value& value = keyframe.values[p];
      if (timeline.props[p].path.empty()) {
        value.data = raw;
      } else if (!PoolValue(timeline.props[p].path.back()->type, raw, value)) {
        return Fail(fmt::format("XUR keyframe value for {} is missing",
                                timeline.props[p].path.back()->name));
      }
    }
  }
  return true;
}

}

std::optional<Document> ParseXur(std::span<const uint8_t> bytes, std::string* error) {
  XurReader reader;
  return reader.Read(bytes, error);
}

}
