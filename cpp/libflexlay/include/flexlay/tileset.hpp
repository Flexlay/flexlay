// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILESET_HPP
#define FLEXLAY_TILESET_HPP

#include "flexlay/tile.hpp"

#include <map>
#include <stdexcept>
#include <vector>

namespace flexlay {

class Tileset
{
public:
  explicit Tileset(int tile_size) : m_tile_size(tile_size) {}

  void add_tile(int tile_id, const Tile& tile) { m_tiles[tile_id] = tile; }

  Tile& create(int tile_id)
  {
    const auto it = m_tiles.find(tile_id);
    if (it == m_tiles.end()) {
      throw std::runtime_error("Tileset::create: unknown tile id");
    }
    return it->second;
  }

  const Tile& create(int tile_id) const
  {
    const auto it = m_tiles.find(tile_id);
    if (it == m_tiles.end()) {
      throw std::runtime_error("Tileset::create: unknown tile id");
    }
    return it->second;
  }

  bool has_tile(int tile_id) const { return m_tiles.find(tile_id) != m_tiles.end(); }
  int get_tile_size() const { return m_tile_size; }

  std::vector<int> get_tiles() const
  {
    std::vector<int> ids;
    ids.reserve(m_tiles.size());
    for (const auto& kv : m_tiles) {
      ids.push_back(kv.first);
    }
    return ids;
  }

private:
  int m_tile_size;
  std::map<int, Tile> m_tiles;
};

} // namespace flexlay

#endif
