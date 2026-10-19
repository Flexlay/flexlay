// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map.hpp"

#include <stdexcept>

namespace flexlay {

void EditorMap::add_layer(std::shared_ptr<Layer> layer, int pos)
{
  if (!layer) {
    throw std::invalid_argument("EditorMap::add_layer: null layer");
  }
  if (pos == -1) {
    m_layers.push_back(std::move(layer));
  } else {
    if (pos < 0 || pos > static_cast<int>(m_layers.size())) {
      throw std::out_of_range("EditorMap::add_layer: pos out of range");
    }
    m_layers.insert(m_layers.begin() + pos, std::move(layer));
  }
  ++m_serial;
}

void EditorMap::draw(GraphicContext& gc) const
{
  if (m_has_bounding_rect) {
    gc.fill_rect(m_bounding_rect, m_background_color);
  }

  for (const auto& layer : m_layers) {
    layer->draw(gc);
  }

  if (m_draw_grid && m_has_bounding_rect) {
    const Color grid(255, 255, 255, 30);
    const int tile = 32;
    const Rect clip = gc.get_clip_rect().to_i();
    const int start_x = clip.left / tile;
    const int start_y = clip.top / tile;
    const int end_x = clip.right / tile + 1;
    const int end_y = clip.bottom / tile + 1;
    for (int x = start_x; x <= end_x; ++x) {
      const float gx = static_cast<float>(x * tile);
      gc.draw_line(gx, m_bounding_rect.top, gx, m_bounding_rect.bottom, grid);
    }
    for (int y = start_y; y <= end_y; ++y) {
      const float gy = static_cast<float>(y * tile);
      gc.draw_line(m_bounding_rect.left, gy, m_bounding_rect.right, gy, grid);
    }
  }
}

} // namespace flexlay
