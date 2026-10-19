// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/object_sprites.hpp"

#include "flexlay/color.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/objmap_sprite_object.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/supertux/sprite_loader.hpp"

#include <unordered_map>

namespace flexlay {
namespace supertux {
namespace {

Color color_for_type(const std::string& type)
{
  unsigned h = 2166136261u;
  for (char c : type) {
    h ^= static_cast<unsigned char>(c);
    h *= 16777619u;
  }
  return Color(static_cast<int>(40 + (h % 180)),
               static_cast<int>(40 + ((h >> 8) % 180)),
               static_cast<int>(40 + ((h >> 16) % 180)), 180);
}

const std::unordered_map<std::string, ObjectVisualInfo>& visual_table()
{
  static const std::unordered_map<std::string, ObjectVisualInfo> table = {
    {"spawnpoint", {"engine/editor/spawnpoint.png", Sizef(32, 32)}},
    {"bonusblock", {"objects/bonus_block/full-0.png", Sizef(32, 32)}},
    {"infoblock", {"engine/editor/infoblock.png", Sizef(32, 32)}},
    {"candle", {"objects/candle/candle-1.png", Sizef(32, 64)}},
    {"snowball", {"creatures/snowball/cpt-snowball.sprite", Sizef(32, 32)}},
    {"bouncing_snowball", {"creatures/bouncing_snowball/bouncing_snowball-editor.png", Sizef(32, 32)}},
    {"mr_tree", {"creatures/mr_tree/mr_tree.sprite", Sizef(64, 64)}},
    {"ghosttree", {"creatures/mr_tree/mr_tree.sprite", Sizef(96, 128)}},
    {"powerup", {"engine/editor/powerup.png", Sizef(32, 32)}},
    {"leveltime", {"engine/editor/clock.png", Sizef(32, 32)}},
    {"scripttrigger", {"engine/editor/point.png", Sizef(64, 32)}},
    {"decal", {"engine/editor/decal.png", Sizef(32, 32)}},
  };
  return table;
}

} // namespace

std::optional<ObjectVisualInfo> object_visual_info(const std::string& type)
{
  const auto& table = visual_table();
  const auto it = table.find(type);
  if (it == table.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::optional<Sprite> try_load_sprite(const std::string& datadir, const std::string& rel_path)
{
  if (datadir.empty() || rel_path.empty()) {
    return std::nullopt;
  }
  // Paths in levels are often "/images/..." or "images/..."
  std::string rel = rel_path;
  if (rel.size() >= 8 && rel.compare(0, 8, "/images/") == 0) {
    rel = rel.substr(8);
  } else if (rel.size() >= 7 && rel.compare(0, 7, "images/") == 0) {
    rel = rel.substr(7);
  }
  const std::string path = datadir + "/images/" + rel;

  // .sprite definition: resolve first frame PNG via sprite_loader
  if (rel.size() > 7 && rel.compare(rel.size() - 7, 7, ".sprite") == 0) {
    if (auto spr = load_sprite_from_def(path)) {
      return spr;
    }
    return std::nullopt;
  }

  try {
    return Sprite(PixelBuffer::from_file(path), rel);
  } catch (...) {
    return std::nullopt;
  }
}

std::shared_ptr<ObjMapObject> make_editor_object(const std::string& type,
                                                const Pointf& pos,
                                                float width,
                                                float height,
                                                const PropertyBag& props,
                                                const std::string& datadir)
{
  std::string image_rel;
  if (auto sp = props.get_as<std::string>("sprite")) {
    image_rel = *sp;
    // .sprite files are not loadable as PNG — try common PNG sibling if needed
    // .sprite paths are resolved by try_load_sprite
  }
  Sizef size(width > 0 ? width : 32.0f, height > 0 ? height : 32.0f);
  if (auto info = object_visual_info(type)) {
    if (image_rel.empty()) {
      image_rel = info->editor_image;
    }
    if (width <= 0) {
      size.width = info->default_size.width;
    }
    if (height <= 0) {
      size.height = info->default_size.height;
    }
  }

  if (auto spr = try_load_sprite(datadir, image_rel)) {
    auto obj = std::make_shared<ObjMapSpriteObject>(*spr, pos);
    obj->set_name(type);
    obj->properties() = props;
    obj->properties().set("type", type);
    if (!image_rel.empty()) {
      obj->properties().set("editor-image", image_rel);
    }
    return obj;
  }

  auto obj = std::make_shared<ObjMapRectObject>(pos, size, color_for_type(type));
  obj->set_name(type);
  obj->properties() = props;
  obj->properties().set("type", type);
  return obj;
}

} // namespace supertux
} // namespace flexlay
