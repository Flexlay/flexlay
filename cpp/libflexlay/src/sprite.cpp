// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/sprite.hpp"

#include "flexlay/math/size.hpp"

namespace flexlay {

void Sprite::draw(float x, float y, GraphicContext& gc) const
{
  if (!m_buffer || m_buffer->empty()) {
    return;
  }
  const float sw = static_cast<float>(m_buffer->width()) * m_scale.first;
  const float sh = static_cast<float>(m_buffer->height()) * m_scale.second;
  const Pointf origin = calc_originf(m_origin, Sizef(sw, sh));
  gc.draw_image(*m_buffer, x - origin.x, y - origin.y, sw, sh);
}

} // namespace flexlay
