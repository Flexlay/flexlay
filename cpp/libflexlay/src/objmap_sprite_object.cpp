// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_sprite_object.hpp"

#include "flexlay/math/size.hpp"

namespace flexlay {

ObjMapSpriteObject::ObjMapSpriteObject(Sprite sprite, const Pointf& pos)
  : m_sprite(std::move(sprite))
{
  m_pos = pos;
}

Rectf ObjMapSpriteObject::get_bound_rect() const
{
  const auto scale = m_sprite.get_scale();
  const float w = static_cast<float>(m_sprite.width()) * scale.first;
  const float h = static_cast<float>(m_sprite.height()) * scale.second;
  const Pointf origin = calc_originf(m_sprite.get_origin(), Sizef(w, h));
  return Rectf::from_ps(m_pos - origin, Sizef(w, h));
}

void ObjMapSpriteObject::draw(GraphicContext& gc)
{
  if (!m_to_draw) {
    return;
  }
  m_sprite.draw(m_pos.x, m_pos.y, gc);
  if (m_selected) {
    gc.draw_rect(get_bound_rect(), Color(255, 255, 255, 255));
  }
}

} // namespace flexlay
