// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILE_BRUSH_HPP
#define FLEXLAY_TILE_BRUSH_HPP

#include "flexlay/field.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace flexlay {

/** Rectangular stamp of tile ids. opaque=false: zero cells are skipped (transparent). */
class TileBrush
{
public:
  TileBrush() : m_width(1), m_height(1), m_data(1, 0) {}

  TileBrush(int width, int height)
    : m_width(std::max(1, width))
    , m_height(std::max(1, height))
    , m_data(static_cast<size_t>(m_width * m_height), 0)
  {
  }

  static TileBrush single_tile(uint32_t tile_id)
  {
    TileBrush b(1, 1);
    b.put(0, 0, tile_id);
    b.set_opaque(true);
    return b;
  }

  static TileBrush filled(int width, int height, uint32_t tile_id, bool opaque = true)
  {
    TileBrush b(width, height);
    for (int i = 0; i < b.m_width * b.m_height; ++i) {
      b.m_data[static_cast<size_t>(i)] = tile_id;
    }
    b.m_opaque = opaque;
    return b;
  }

  /** Copy a region from a field (top-left x,y size w×h). Transparent zeros. */
  static TileBrush from_field(const Field& field, int x, int y, int w, int h)
  {
    w = std::max(1, w);
    h = std::max(1, h);
    TileBrush b(w, h);
    for (int iy = 0; iy < h; ++iy) {
      for (int ix = 0; ix < w; ++ix) {
        const int sx = x + ix;
        const int sy = y + iy;
        if (sx >= 0 && sy >= 0 && sx < field.width() && sy < field.height()) {
          b.put(ix, iy, field.at(sx, sy));
        }
      }
    }
    b.set_opaque(false);
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
