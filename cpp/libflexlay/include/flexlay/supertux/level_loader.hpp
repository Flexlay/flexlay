// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_LEVEL_LOADER_HPP
#define FLEXLAY_SUPERTUX_LEVEL_LOADER_HPP

/**
 * Minimal SuperTux level loader targeting SuperTux-Origins format:
 * - Levels: (supertux-level ...) typically in .stlv (legacy .stl still ok)
 * - Tilesets: (supertux-tileset ...) in .stts
 *
 * Loads first sector tilemaps + a subset of game objects into EditorMap.
 */

#include "flexlay/editor_map.hpp"
#include "flexlay/property_bag.hpp"
#include "flexlay/tileset.hpp"

#include <memory>
#include <string>
#include <vector>

namespace flexlay {
namespace supertux {

struct LoadedTilemap
{
  std::string name;
  int width = 0;
  int height = 0;
  bool solid = false;
  int z_pos = 0;
  std::vector<uint32_t> tiles; // row-major, size width*height
};

/** Generic game object: type tag + property bag (x/y/width/height extracted). */
struct LoadedObject
{
  std::string type;
  float x = 0;
  float y = 0;
  float width = 32;
  float height = 32;
  PropertyBag props;
};

struct LoadedLevel
{
  std::string name;
  int version = 0;
  std::string sector_name;
  std::vector<LoadedTilemap> tilemaps;
  std::vector<LoadedObject> objects;
};

/** Parse level from sexpr text or file path. */
LoadedLevel load_level_text(const std::string& text);
LoadedLevel load_level_file(const std::string& path);

/**
 * Build an EditorMap from loaded tilemaps + objects.
 * Objects become ObjMapRectObject entries on an "objects" ObjectLayer.
 */
std::shared_ptr<EditorMap> editor_map_from_level(const LoadedLevel& level,
                                                 std::shared_ptr<Tileset> tileset);

} // namespace supertux
} // namespace flexlay

#endif
