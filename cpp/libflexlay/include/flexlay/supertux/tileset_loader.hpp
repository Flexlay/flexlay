// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_TILESET_LOADER_HPP
#define FLEXLAY_SUPERTUX_TILESET_LOADER_HPP

/**
 * Minimal SuperTux-Origins tileset loader (.stts / supertux-tileset).
 * Parses (tile ...) definitions and bulk (tiles ...) id lists with sheet cropping.
 * Image loading is optional (datadir + Qt PixelBuffer::from_file).
 */

#include "flexlay/tileset.hpp"

#include <memory>
#include <string>
#include <vector>

namespace flexlay {
namespace supertux {

struct TileGroupInfo
{
  std::string name;
  std::vector<int> tiles;
};

struct LoadedTilesetMeta
{
  std::string filename;
  std::vector<TileGroupInfo> tilegroups;
  /** Number of tile definitions registered (id != 0). */
  int tile_count = 0;
};

struct TilesetLoadOptions
{
  /** SuperTux data root (contains images/). Empty = no image load. */
  std::string datadir;
  /** Tile pixel size (Origins default 32). */
  int tile_size = 32;
  /** If true and datadir set, load first image per tile (slow for full set). */
  bool load_images = false;
};

/**
 * Parse tileset from sexpr text. Returns tileset + metadata.
 * Without load_images, tiles get solid color placeholders and optional filename.
 */
std::pair<std::shared_ptr<Tileset>, LoadedTilesetMeta>
load_tileset_text(const std::string& text, const TilesetLoadOptions& opts = {});

std::pair<std::shared_ptr<Tileset>, LoadedTilesetMeta>
load_tileset_file(const std::string& path, const TilesetLoadOptions& opts = {});

} // namespace supertux
} // namespace flexlay

#endif
