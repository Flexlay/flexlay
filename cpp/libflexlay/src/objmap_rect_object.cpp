// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_rect_object.hpp"

#include <algorithm>
#include <cmath>

namespace flexlay {

ObjMapRectObject::ObjMapRectObject(const Pointf& pos, const Sizef& size, const Color& color)
  : m_size(size)
  , m_color(color)
{
  m_pos = pos;
}

void ObjMapRectObject::set_size(const Sizef& size)
{
  m_size.width = std::max(min_size, size.width);
  m_size.height = std::max(min_size, size.height);
}

Rectf ObjMapRectObject::get_bound_rect() const
{
  return Rectf::from_ps(m_pos, m_size);
}

Rectf ObjMapRectObject::handle_rect(ResizeHandle handle) const
{
  const Rectf r = get_bound_rect();
  const float hs = handle_size;
  Pointf c;
  switch (handle) {
    case ResizeHandle::top_left:
      c = Pointf(r.left, r.top);
      break;
    case ResizeHandle::top_right:
      c = Pointf(r.right, r.top);
      break;
    case ResizeHandle::bottom_left:
      c = Pointf(r.left, r.bottom);
      break;
    case ResizeHandle::bottom_right:
      c = Pointf(r.right, r.bottom);
      break;
    default:
      return Rectf::zero();
  }
  return Rectf(c.x - hs / 2, c.y - hs / 2, c.x + hs / 2, c.y + hs / 2);
}

ResizeHandle ObjMapRectObject::hit_handle(const Pointf& world_pos) const
{
  if (!m_selected) {
    return ResizeHandle::none;
  }
  const ResizeHandle handles[] = {
    ResizeHandle::top_left, ResizeHandle::top_right,
    ResizeHandle::bottom_left, ResizeHandle::bottom_right,
  };
  for (auto h : handles) {
    if (handle_rect(h).is_inside(world_pos)) {
      return h;
    }
  }
  return ResizeHandle::none;
}

void ObjMapRectObject::resize_to_handle(ResizeHandle handle, const Pointf& world_pos)
{
  Rectf r = get_bound_rect();
  switch (handle) {
    case ResizeHandle::top_left:
      r.left = world_pos.x;
      r.top = world_pos.y;
      break;
    case ResizeHandle::top_right:
      r.right = world_pos.x;
      r.top = world_pos.y;
      break;
    case ResizeHandle::bottom_left:
      r.left = world_pos.x;
      r.bottom = world_pos.y;
      break;
    case ResizeHandle::bottom_right:
      r.right = world_pos.x;
      r.bottom = world_pos.y;
      break;
    default:
      return;
  }
  r.normalize();
  // enforce min size after normalize
  if (r.width() < min_size) {
    r.right = r.left + min_size;
  }
  if (r.height() < min_size) {
    r.bottom = r.top + min_size;
  }
  m_pos = Pointf(r.left, r.top);
  m_size = Sizef(r.width(), r.height());
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
    const Color hc(255, 220, 40, 255);
    for (auto h : {ResizeHandle::top_left, ResizeHandle::top_right,
                   ResizeHandle::bottom_left, ResizeHandle::bottom_right}) {
      gc.fill_rect(handle_rect(h), hc);
      gc.draw_rect(handle_rect(h), Color(0, 0, 0, 255));
    }
  } else {
    gc.draw_rect(r, Color(m_color.r, m_color.g, m_color.b, 255));
  }
}

} // namespace flexlay
