// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/level_loader.hpp"

#include "flexlay/tilemap_layer.hpp"
#include "flexlay/util/sexpr.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

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
  tm.width = child_int(node, "width");
  tm.height = child_int(node, "height");
  tm.solid = child_bool(node, "solid");
  tm.z_pos = child_int(node, "z-pos");
  tm.name = child_atom(node, "name");

  const SExpr* tiles = find_child(node, "tiles");
  if (tiles && tiles->is_list()) {
    for (size_t i = 1; i < tiles->as_list().size(); ++i) {
      const auto& cell = tiles->as_list()[i];
      if (!cell.is_atom()) {
        continue;
      }
      try {
        tm.tiles.push_back(static_cast<uint32_t>(std::stoul(cell.as_atom())));
      } catch (...) {
        tm.tiles.push_back(0);
      }
    }
  }

  const size_t expect = static_cast<size_t>(std::max(0, tm.width) * std::max(0, tm.height));
  if (tm.tiles.size() < expect) {
    tm.tiles.resize(expect, 0);
  } else if (tm.tiles.size() > expect && expect > 0) {
    tm.tiles.resize(expect);
  }
  return tm;
}

void collect_tilemaps(const SExpr& objects_node, std::vector<LoadedTilemap>& out)
{
  if (!objects_node.is_list()) {
    return;
  }
  for (size_t i = 1; i < objects_node.as_list().size(); ++i) {
    const auto& obj = objects_node.as_list()[i];
    if (obj.tag() == "tilemap") {
      out.push_back(parse_tilemap(obj));
    }
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

  const SExpr* sectors = find_child(root, "sectors");
  if (!sectors) {
    return level;
  }

  for (size_t i = 1; i < sectors->as_list().size(); ++i) {
    const auto& sector = sectors->as_list()[i];
    if (sector.tag() != "sector") {
      continue;
    }
    level.sector_name = child_atom(sector, "name", "main");
    const SExpr* objects = find_child(sector, "objects");
    if (objects) {
      collect_tilemaps(*objects, level.tilemaps);
    }
    break; // first sector only
  }

  std::stable_sort(level.tilemaps.begin(), level.tilemaps.end(),
                   [](const LoadedTilemap& a, const LoadedTilemap& b) {
                     return a.z_pos < b.z_pos;
                   });
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
  return load_level_text(ss.str());
}

std::shared_ptr<EditorMap> editor_map_from_level(const LoadedLevel& level,
                                                 std::shared_ptr<Tileset> tileset)
{
  auto map = std::make_shared<EditorMap>();
  int max_w = 0;
  int max_h = 0;
  const int ts = tileset ? tileset->get_tile_size() : 32;

  for (const auto& tm : level.tilemaps) {
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

  map->set_bounding_rect(Rectf(0, 0, static_cast<float>(max_w * ts),
                               static_cast<float>(max_h * ts)));
  map->set_background_color(Color(45, 45, 55));
  return map;
}

} // namespace supertux
} // namespace flexlay
