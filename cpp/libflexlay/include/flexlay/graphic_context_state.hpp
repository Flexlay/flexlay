// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GRAPHIC_CONTEXT_STATE_HPP
#define FLEXLAY_GRAPHIC_CONTEXT_STATE_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/math/size.hpp"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace flexlay {

class GraphicContextState
{
public:
  GraphicContextState() = default;
  GraphicContextState(int width, int height) : m_width(width), m_height(height) {}

  void set_size(int width, int height) { m_width = width; m_height = height; }
  int get_width() const { return m_width; }
  int get_height() const { return m_height; }

  void set_offset(const Pointf& offset) { m_offset = offset; }
  Pointf get_offset() const { return m_offset; }

  float get_zoom() const { return m_zoom; }
  void set_zoom(float z) { m_zoom = z; }

  void set_zoom(float z, const Pointf& pos)
  {
    const float old_zoom = m_zoom;
    m_zoom = z;
    m_offset.x = pos.x / m_zoom - pos.x / old_zoom + m_offset.x;
    m_offset.y = pos.y / m_zoom - pos.y / old_zoom + m_offset.y;
  }

  void set_rotation(float angle) { m_rotation = angle; }
  float get_rotation() const { return m_rotation; }

  void set_pos(const Pointf& pos)
  {
    m_offset.x = -pos.x + (static_cast<float>(m_width) / 2.0f / m_zoom);
    m_offset.y = -pos.y + (static_cast<float>(m_height) / 2.0f / m_zoom);
  }

  Pointf get_pos() const
  {
    return Pointf(-m_offset.x + (static_cast<float>(m_width) / 2.0f / m_zoom),
                  -m_offset.y + (static_cast<float>(m_height) / 2.0f / m_zoom));
  }

  Rectf get_clip_rect() const
  {
    return Rectf::from_ps(Pointf(-m_offset.x, -m_offset.y),
                          Sizef(static_cast<float>(m_width) / m_zoom,
                                static_cast<float>(m_height) / m_zoom));
  }

  void push(GraphicContext& gc) const
  {
    gc.push_modelview();
    gc.translate(static_cast<float>(m_width) / 2.0f, static_cast<float>(m_height) / 2.0f);
    gc.rotate(m_rotation);
    gc.scale(m_zoom, m_zoom);
    gc.translate(-static_cast<float>(m_width) / 2.0f / m_zoom,
                 -static_cast<float>(m_height) / 2.0f / m_zoom);
    gc.translate(m_offset.x, m_offset.y);
  }

  void pop(GraphicContext& gc) const { gc.pop_modelview(); }

  Pointf screen2world(Pointf pos) const
  {
    const float sa = std::sin(-m_rotation / 180.0f * static_cast<float>(M_PI));
    const float ca = std::cos(-m_rotation / 180.0f * static_cast<float>(M_PI));
    const float dx = pos.x - static_cast<float>(m_width) / 2.0f;
    const float dy = pos.y - static_cast<float>(m_height) / 2.0f;
    pos.x = static_cast<float>(m_width) / 2.0f + (ca * dx - sa * dy);
    pos.y = static_cast<float>(m_height) / 2.0f + (sa * dx + ca * dy);
    return Pointf((pos.x / m_zoom) - m_offset.x, (pos.y / m_zoom) - m_offset.y);
  }

  void zoom_to(const Rectf& rect)
  {
    const float center_x = (rect.left + rect.right) / 2.0f;
    const float center_y = (rect.top + rect.bottom) / 2.0f;
    const float width = rect.right - rect.left;
    const float height = rect.bottom - rect.top;
    const float screen_relation = static_cast<float>(m_height) / static_cast<float>(m_width);
    const float rect_relation = height / width;
    if (rect_relation < screen_relation) {
      m_zoom = static_cast<float>(m_width) / width;
    } else {
      m_zoom = static_cast<float>(m_height) / height;
    }
    m_offset.x = (static_cast<float>(m_width) / (2.0f * m_zoom)) - center_x;
    m_offset.y = (static_cast<float>(m_height) / (2.0f * m_zoom)) - center_y;
  }

private:
  int m_width = 0;
  int m_height = 0;
  Pointf m_offset;
  float m_zoom = 1.0f;
  float m_rotation = 0.0f;
};

} // namespace flexlay

#endif
