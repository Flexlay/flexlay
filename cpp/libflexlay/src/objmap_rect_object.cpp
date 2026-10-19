// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_rect_object.hpp"

namespace flexlay {

ObjMapRectObject::ObjMapRectObject(const Pointf& pos, const Sizef& size, const Color& color)
  : m_size(size)
  , m_color(color)
{
  m_pos = pos;
}

Rectf ObjMapRectObject::get_bound_rect() const
{
  return Rectf::from_ps(m_pos, m_size);
}

void ObjMapRectObject::draw(GraphicContext& gc)
{
  if (!m_to_draw) {
    return;
  }
  const Rectf r = get_bound_rect();
  gc.fill_rect(r, m_color);
  if (m_selected) {
    gc.draw_rect(r, Color(255, 255, 255, 255));
    // selection handles (corners)
    const float hs = 4.0f;
    const Color hc(255, 255, 0, 255);
    gc.fill_rect(Rectf(r.left - hs / 2, r.top - hs / 2, r.left + hs / 2, r.top + hs / 2), hc);
    gc.fill_rect(Rectf(r.right - hs / 2, r.top - hs / 2, r.right + hs / 2, r.top + hs / 2), hc);
    gc.fill_rect(Rectf(r.left - hs / 2, r.bottom - hs / 2, r.left + hs / 2, r.bottom + hs / 2), hc);
    gc.fill_rect(Rectf(r.right - hs / 2, r.bottom - hs / 2, r.right + hs / 2, r.bottom + hs / 2), hc);
  } else {
    gc.draw_rect(r, Color(m_color.r, m_color.g, m_color.b, 255));
  }
}

} // namespace flexlay
