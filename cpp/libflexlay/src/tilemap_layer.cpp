// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tilemap_layer.hpp"

#include "flexlay/math/rect.hpp"

#include <algorithm>

namespace flexlay {

TilemapLayer::TilemapLayer(std::shared_ptr<Tileset> tileset, int width, int height)
  : m_tileset(std::move(tileset))
  , m_field(Field::from_size(width, height))
{
}

void TilemapLayer::draw(GraphicContext& gc, const Point& pos)
{
  if (m_hidden) {
    return;
  }

  const int tile_size = m_tileset->get_tile_size();
  const Rect clip = gc.get_clip_rect().to_i();

  const int start_x = std::max(0, (clip.left - pos.x) / tile_size);
  const int start_y = std::max(0, (clip.top - pos.y) / tile_size);
  const int end_x = std::min(m_field.width(), (clip.right - pos.x) / tile_size + 1);
  const int end_y = std::min(m_field.height(), (clip.bottom - pos.y) / tile_size + 1);

  for (int y = start_y; y < end_y; ++y) {
    for (int x = start_x; x < end_x; ++x) {
      const uint32_t tile_id = m_field.at(x, y);
      if (tile_id == 0) {
        continue;
      }

      const float px = static_cast<float>(pos.x + x * tile_size);
      const float py = static_cast<float>(pos.y + y * tile_size);
      const Rectf cell(px, py, px + static_cast<float>(tile_size), py + static_cast<float>(tile_size));

      if (!m_tileset->has_tile(static_cast<int>(tile_id))) {
        gc.fill_rect(cell, Color(255, 0, 255, 180));
        continue;
      }

      const Tile& tile = m_tileset->create(static_cast<int>(tile_id));
      if (tile.is_transparent()) {
        continue;
      }

      Color color = tile.get_color();
      if (m_foreground_color != Color(255, 255, 255, 255)) {
        color = Color((color.r * m_foreground_color.r) / 255,
                      (color.g * m_foreground_color.g) / 255,
                      (color.b * m_foreground_color.b) / 255,
                      (color.a * m_foreground_color.a) / 255);
      }
      gc.fill_rect(cell, color);

      if (m_draw_attribute) {
        gc.fill_rect(cell, tile.get_attribute_color());
      }
    }
  }

  if (m_draw_grid) {
    const Color grid_color(255, 255, 255, 40);
    const float map_w = static_cast<float>(m_field.width() * tile_size);
    const float map_h = static_cast<float>(m_field.height() * tile_size);
    for (int x = 0; x <= m_field.width(); ++x) {
      const float gx = static_cast<float>(pos.x + x * tile_size);
      gc.draw_line(gx, static_cast<float>(pos.y), gx, static_cast<float>(pos.y) + map_h, grid_color);
    }
    for (int y = 0; y <= m_field.height(); ++y) {
      const float gy = static_cast<float>(pos.y + y * tile_size);
      gc.draw_line(static_cast<float>(pos.x), gy, static_cast<float>(pos.x) + map_w, gy, grid_color);
    }
  }
}

} // namespace flexlay
