// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_LEVEL_LOADER_HPP
#define FLEXLAY_SUPERTUX_LEVEL_LOADER_HPP

/**
 * Minimal SuperTux level loader targeting SuperTux-Origins format.
 * Supports multiple sectors: all are parsed; one is "active" for the editor map.
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

struct LoadedSector
{
  std::string name{"main"};
  float gravity = 10.0f;
  float ambient_r = 1.0f;
  float ambient_g = 1.0f;
  float ambient_b = 1.0f;
  float ambient_a = 1.0f;
  bool has_ambient_light = false;
  bool has_gravity = false;
  std::vector<LoadedTilemap> tilemaps;
  std::vector<LoadedObject> objects;
};

struct LoadedLevel
{
  std::string name;
  int version = 0;
  std::string author;
  std::string license;
  std::string contact;
  std::string tileset_path;
  int target_time = 0;

  std::vector<LoadedSector> sectors;
  /** Index into sectors for the sector currently shown in the editor. */
  size_t active_sector = 0;
  /** True when loaded from .stwm or marked as worldmap. */
  bool is_worldmap = false;

  // --- convenience views of the active sector (legacy / UI helpers) ---
  std::string sector_name() const
  {
    return active() ? active()->name : std::string{"main"};
  }
  const std::vector<LoadedTilemap>& tilemaps() const
  {
    static const std::vector<LoadedTilemap> empty;
    return active() ? active()->tilemaps : empty;
  }
  const std::vector<LoadedObject>& objects() const
  {
    static const std::vector<LoadedObject> empty;
    return active() ? active()->objects : empty;
  }
  float gravity() const { return active() ? active()->gravity : 10.0f; }
  bool has_gravity() const { return active() && active()->has_gravity; }
  bool has_ambient_light() const { return active() && active()->has_ambient_light; }
  float ambient_r() const { return active() ? active()->ambient_r : 1.0f; }
  float ambient_g() const { return active() ? active()->ambient_g : 1.0f; }
  float ambient_b() const { return active() ? active()->ambient_b : 1.0f; }
  float ambient_a() const { return active() ? active()->ambient_a : 1.0f; }

  const LoadedSector* active() const
  {
    if (sectors.empty() || active_sector >= sectors.size()) {
      return nullptr;
    }
    return &sectors[active_sector];
  }
  LoadedSector* active()
  {
    if (sectors.empty() || active_sector >= sectors.size()) {
      return nullptr;
    }
    return &sectors[active_sector];
  }

  /** Select sector by name; returns false if not found. */
  bool select_sector(const std::string& name)
  {
    for (size_t i = 0; i < sectors.size(); ++i) {
      if (sectors[i].name == name) {
        active_sector = i;
        return true;
      }
    }
    return false;
  }

  std::vector<std::string> sector_names() const
  {
    std::vector<std::string> out;
    out.reserve(sectors.size());
    for (const auto& s : sectors) {
      out.push_back(s.name);
    }
    return out;
  }
};

/** Parse level from sexpr text or file path. Loads all sectors; active = first. */
LoadedLevel load_level_text(const std::string& text);
LoadedLevel load_level_file(const std::string& path);

/**
 * Build an EditorMap from the level's active sector.
 */
std::shared_ptr<EditorMap> editor_map_from_level(const LoadedLevel& level,
                                                 std::shared_ptr<Tileset> tileset,
                                                 const std::string& datadir = {});

} // namespace supertux
} // namespace flexlay

#endif
