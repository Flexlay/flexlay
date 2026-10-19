// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILE_BRUSH_HPP
#define FLEXLAY_TILE_BRUSH_HPP

#include "flexlay/field.hpp"

#include <cstdint>
#include <vector>

namespace flexlay {

/** Rectangular stamp of tile ids. opaque=false: zero cells are skipped (transparent). */
class TileBrush
{
public:
  TileBrush() : m_width(1), m_height(1), m_data(1, 0) {}

  TileBrush(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_data(static_cast<size_t>(width * height), 0)
  {
  }

  static TileBrush single_tile(uint32_t tile_id)
  {
    TileBrush b(1, 1);
    b.put(0, 0, tile_id);
    b.set_opaque(true);
    return b;
  }

  /** Filled rectangle of the same tile id. */
  static TileBrush filled(int width, int height, uint32_t tile_id, bool opaque = true)
  {
    TileBrush b(width, height);
    for (int i = 0; i < width * height; ++i) {
      b.m_data[static_cast<size_t>(i)] = tile_id;
    }
    b.m_opaque = opaque;
    return b;
  }

  int width() const { return m_width; }
  int height() const { return m_height; }

  uint32_t at(int x, int y) const
  {
    return m_data[static_cast<size_t>(y * m_width + x)];
  }

  void put(int x, int y, uint32_t value)
  {
    m_data[static_cast<size_t>(y * m_width + x)] = value;
  }

  void set_opaque(bool v) { m_opaque = v; }
  bool is_opaque() const { return m_opaque; }

private:
  int m_width;
  int m_height;
  std::vector<uint32_t> m_data;
  bool m_opaque = false;
};

} // namespace flexlay

#endif
