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
 * Level metadata (author, license, tileset path, …) is preserved for save.
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
  std::string author;
  std::string license;
  std::string contact;
  std::string tileset_path; // e.g. "images/tiles.stts"
  int target_time = 0;      // 0 = unset
  std::string sector_name;
  /** Sector gravity (SuperTux default 10). */
  float gravity = 10.0f;
  /** Sector ambient light RGBA in 0..1 (default white). */
  float ambient_r = 1.0f;
  float ambient_g = 1.0f;
  float ambient_b = 1.0f;
  float ambient_a = 1.0f;
  bool has_ambient_light = false;
  bool has_gravity = false;
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
                                                 std::shared_ptr<Tileset> tileset,
                                                 const std::string& datadir = {});

} // namespace supertux
} // namespace flexlay

#endif
