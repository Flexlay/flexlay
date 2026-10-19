// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/level_loader.hpp"

#include "flexlay/color.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/supertux/object_sprites.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/util/sexpr.hpp"

#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace flexlay {
namespace supertux {
namespace {

const SExpr* find_child(const SExpr& list, const std::string& tag)
{
  if (!list.is_list()) {
    return nullptr;
  }
  for (const auto& child : list.as_list()) {
    if (child.is_list() && child.tag() == tag) {
      return &child;
    }
  }
  return nullptr;
}

std::string child_atom(const SExpr& list, const std::string& tag, const std::string& def = {})
{
  const SExpr* c = find_child(list, tag);
  if (!c || c->as_list().size() < 2 || !c->as_list()[1].is_atom()) {
    return def;
  }
  return c->as_list()[1].as_atom();
}

int child_int(const SExpr& list, const std::string& tag, int def = 0)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  try {
    return std::stoi(a);
  } catch (...) {
    return def;
  }
}

float child_float(const SExpr& list, const std::string& tag, float def = 0.0f)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  try {
    return std::stof(a);
  } catch (...) {
    return def;
  }
}

bool child_bool(const SExpr& list, const std::string& tag, bool def = false)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  return a == "#t" || a == "true" || a == "1";
}

LoadedTilemap parse_tilemap(const SExpr& node)
{
  LoadedTilemap tm;
  tm.name = child_atom(node, "name");
  tm.width = child_int(node, "width");
  tm.height = child_int(node, "height");
  tm.solid = child_bool(node, "solid");
  tm.z_pos = child_int(node, "z-pos");

  const SExpr* tiles = find_child(node, "tiles");
  if (tiles && tiles->is_list()) {
    for (size_t i = 1; i < tiles->as_list().size(); ++i) {
      const auto& a = tiles->as_list()[i];
      if (!a.is_atom()) {
        continue;
      }
      try {
        tm.tiles.push_back(static_cast<uint32_t>(std::stoul(a.as_atom())));
      } catch (...) {
        tm.tiles.push_back(0);
      }
    }
  }
  return tm;
}

/** Tags that are sector infrastructure, not placeable game objects. */
bool is_skipped_object_tag(const std::string& tag)
{
  static const std::unordered_set<std::string> skip = {
    "tilemap", "camera", "ambient-light", "gravity", "music", "init-script",
    "background", "gradient", "thunderstorm", "particles-*",
    "name", "objects", "width", "height"
  };
  if (skip.count(tag)) {
    return true;
  }
  // particle systems / backgrounds often start with these
  if (tag.find("particles") != std::string::npos) {
    return true;
  }
  if (tag.rfind("background", 0) == 0) {
    return true;
  }
  return false;
}

PropertyValue atom_to_prop(const std::string& a)
{
  if (a == "#t" || a == "true") {
    return true;
  }
  if (a == "#f" || a == "false") {
    return false;
  }
  try {
    if (a.find('.') != std::string::npos) {
      return std::stof(a);
    }
    // large numbers stay as int if they fit
    long long v = std::stoll(a);
    if (v >= static_cast<long long>(std::numeric_limits<int>::min()) &&
        v <= static_cast<long long>(std::numeric_limits<int>::max())) {
      return static_cast<int>(v);
    }
  } catch (...) {
  }
  return a;
}

LoadedObject parse_object(const SExpr& node)
{
  LoadedObject obj;
  obj.type = node.tag();
  obj.x = child_float(node, "x");
  obj.y = child_float(node, "y");
  obj.width = child_float(node, "width", 32.0f);
  obj.height = child_float(node, "height", 32.0f);

  if (!node.is_list()) {
    return obj;
  }
  for (size_t i = 1; i < node.as_list().size(); ++i) {
    const auto& child = node.as_list()[i];
    if (!child.is_list() || child.as_list().empty()) {
      continue;
    }
    const std::string key = child.tag();
    if (key.empty() || key == "x" || key == "y" || key == "width" || key == "height") {
      continue;
    }
    // single atom value
    if (child.as_list().size() == 2 && child.as_list()[1].is_atom()) {
      obj.props.set(key, atom_to_prop(child.as_list()[1].as_atom()));
      continue;
    }
    // multi-value: store as space-joined string for now
    if (child.as_list().size() > 2) {
      std::ostringstream ss;
      for (size_t j = 1; j < child.as_list().size(); ++j) {
        if (j > 1) {
          ss << ' ';
        }
        if (child.as_list()[j].is_atom()) {
          ss << child.as_list()[j].as_atom();
        }
      }
      obj.props.set(key, ss.str());
    }
  }
  return obj;
}

void parse_ambient_light(const SExpr& node, LoadedSector& sector)
{
  // (ambient-light 1 1 1 1) or (ambient-light (color 1 1 1 1))
  const SExpr* color = find_child(node, "color");
  const SExpr* src = color ? color : &node;
  if (!src->is_list() || src->as_list().size() < 2) {
    return;
  }
  std::vector<float> comps;
  for (size_t i = 1; i < src->as_list().size(); ++i) {
    if (!src->as_list()[i].is_atom()) {
      continue;
    }
    try {
      comps.push_back(std::stof(src->as_list()[i].as_atom()));
    } catch (...) {
    }
  }
  if (comps.size() >= 3) {
    sector.ambient_r = comps[0];
    sector.ambient_g = comps[1];
    sector.ambient_b = comps[2];
    sector.ambient_a = comps.size() >= 4 ? comps[3] : 1.0f;
    sector.has_ambient_light = true;
  }
}

void collect_objects(const SExpr& objects_node, LoadedSector& sector)
{
  if (!objects_node.is_list()) {
    return;
  }
  for (size_t i = 1; i < objects_node.as_list().size(); ++i) {
    const auto& obj = objects_node.as_list()[i];
    if (!obj.is_list()) {
      continue;
    }
    const std::string tag = obj.tag();
    if (tag == "tilemap") {
      sector.tilemaps.push_back(parse_tilemap(obj));
      continue;
    }
    if (tag == "gravity") {
      if (obj.as_list().size() >= 2 && obj.as_list()[1].is_atom()) {
        try {
          sector.gravity = std::stof(obj.as_list()[1].as_atom());
          sector.has_gravity = true;
        } catch (...) {
        }
      }
      continue;
    }
    if (tag == "ambient-light") {
      parse_ambient_light(obj, sector);
      continue;
    }
    if (is_skipped_object_tag(tag) || tag.empty()) {
      continue;
    }
    sector.objects.push_back(parse_object(obj));
  }
}


} // namespace

LoadedLevel load_level_text(const std::string& text)
{
  const SExpr root = sexpr_parse(text);
  if (root.tag() != "supertux-level") {
    throw std::runtime_error("load_level: root is not supertux-level");
  }

  LoadedLevel level;
  level.version = child_int(root, "version");
  level.name = child_atom(root, "name");
  level.author = child_atom(root, "author");
  level.license = child_atom(root, "license");
  level.contact = child_atom(root, "contact");
  level.tileset_path = child_atom(root, "tileset");
  level.target_time = child_int(root, "target-time", 0);

  auto finalize_sector = [](LoadedSector& sector) {
    if (sector.name.empty()) {
      sector.name = "main";
    }
    std::stable_sort(sector.tilemaps.begin(), sector.tilemaps.end(),
                     [](const LoadedTilemap& a, const LoadedTilemap& b) {
                       return a.z_pos < b.z_pos;
                     });
  };

  auto parse_one_sector = [&](const SExpr& sector_node) {
    LoadedSector sector;
    sector.name = child_atom(sector_node, "name", "main");
    // Classic: (objects (tilemap ...) ...)
    const SExpr* objects = find_child(sector_node, "objects");
    if (objects) {
      collect_objects(*objects, sector);
    } else {
      // Modern SuperTux / worldmap: tilemaps & objects are direct sector children
      collect_objects(sector_node, sector);
    }
    finalize_sector(sector);
    level.sectors.push_back(std::move(sector));
  };

  // Nested (sectors (sector ...)) form (tests / older packs)
  const SExpr* sectors = find_child(root, "sectors");
  if (sectors && sectors->is_list()) {
    for (size_t i = 1; i < sectors->as_list().size(); ++i) {
      const auto& sector_node = sectors->as_list()[i];
      if (sector_node.is_list() && sector_node.tag() == "sector") {
        parse_one_sector(sector_node);
      }
    }
  }

  // Modern: (sector ...) direct children of root
  if (root.is_list()) {
    for (size_t i = 1; i < root.as_list().size(); ++i) {
      const auto& child = root.as_list()[i];
      if (child.is_list() && child.tag() == "sector") {
        parse_one_sector(child);
      }
    }
  }

  level.active_sector = 0;
  return level;
}

LoadedLevel load_level_file(const std::string& path)
{
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("load_level_file: cannot open " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  LoadedLevel level = load_level_text(ss.str());
  // Worldmap files use tile-grid coordinates for objects
  if (path.size() >= 5 && path.compare(path.size() - 5, 5, ".stwm") == 0) {
    level.is_worldmap = true;
  }
  return level;
}

std::shared_ptr<EditorMap> editor_map_from_level(const LoadedLevel& level,
                                                 std::shared_ptr<Tileset> tileset,
                                                 const std::string& datadir)
{
  auto map = std::make_shared<EditorMap>();
  const int ts = tileset ? tileset->get_tile_size() : 32;
  const LoadedSector* sector = level.active();
  if (!sector) {
    map->set_bounding_rect(Rectf(0, 0, static_cast<float>(ts), static_cast<float>(ts)));
    map->set_background_color(Color(100, 150, 200));
    return map;
  }

  int max_w = 0;
  int max_h = 0;
  for (const auto& tm : sector->tilemaps) {
    max_w = std::max(max_w, tm.width);
    max_h = std::max(max_h, tm.height);
    if (!tileset || tm.width <= 0 || tm.height <= 0) {
      continue;
    }
    auto layer = std::make_shared<TilemapLayer>(tileset, tm.width, tm.height);
    {
      std::string nm = tm.name.empty() ? ("tilemap z=" + std::to_string(tm.z_pos)) : tm.name;
      if (tm.solid) {
        nm += " [solid]";
      }
      layer->set_name(nm);
    }
    layer->set_draw_grid(true);
    for (int y = 0; y < tm.height; ++y) {
      for (int x = 0; x < tm.width; ++x) {
        const size_t i = static_cast<size_t>(y * tm.width + x);
        if (i < tm.tiles.size()) {
          layer->field().put(x, y, tm.tiles[i]);
        }
      }
    }
    map->add_layer(layer);
  }

  auto object_layer = std::make_shared<ObjectLayer>();
  object_layer->set_name("objects");
  const float pos_scale = level.is_worldmap ? 32.0f : 1.0f;
  for (const auto& lo : sector->objects) {
    auto obj = make_editor_object(lo.type, Pointf(lo.x * pos_scale, lo.y * pos_scale),
                                  lo.width, lo.height, lo.props, datadir);
    object_layer->add_object(obj);
  }
  map->add_layer(object_layer);

  map->set_bounding_rect(Rectf(0, 0, static_cast<float>(std::max(max_w, 1) * ts),
                               static_cast<float>(std::max(max_h, 1) * ts)));
  // SuperTux-ish sky; blend ambient light when the sector provides it
  Color bg(100, 150, 200);
  if (sector->has_ambient_light) {
    bg = Color(static_cast<int>(sector->ambient_r * 180.0f + 40.0f),
               static_cast<int>(sector->ambient_g * 180.0f + 40.0f),
               static_cast<int>(sector->ambient_b * 200.0f + 30.0f));
  }
  map->set_background_color(bg);
  map->set_draw_grid(true);
  return map;
}

} // namespace supertux
} // namespace flexlay
