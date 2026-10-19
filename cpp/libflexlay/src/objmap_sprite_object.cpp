// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_sprite_object.hpp"

#include "flexlay/color.hpp"
#include "flexlay/math/rect.hpp"

namespace flexlay {

ObjMapSpriteObject::ObjMapSpriteObject(Sprite sprite, const Pointf& pos)
  : m_sprite(std::move(sprite))
{
  m_pos = pos;
}

Sprite& ObjMapSpriteObject::current_sprite()
{
  if (!m_frames.empty()) {
    const int i = m_frame_index % static_cast<int>(m_frames.size());
    return m_frames[static_cast<size_t>(i)];
  }
  return m_sprite;
}

const Sprite& ObjMapSpriteObject::current_sprite() const
{
  if (!m_frames.empty()) {
    const int i = m_frame_index % static_cast<int>(m_frames.size());
    return m_frames[static_cast<size_t>(i)];
  }
  return m_sprite;
}

void ObjMapSpriteObject::set_sprite(Sprite sprite)
{
  m_sprite = std::move(sprite);
  m_frames.clear();
  m_frame_index = 0;
  m_anim_time = 0;
}

void ObjMapSpriteObject::set_frames(std::vector<Sprite> frames, float fps)
{
  m_frames = std::move(frames);
  m_fps = fps > 0.0f ? fps : 15.0f;
  m_frame_index = 0;
  m_anim_time = 0;
  if (!m_frames.empty()) {
    m_sprite = m_frames.front();
  }
}

void ObjMapSpriteObject::advance_animation(float dt_seconds)
{
  if (m_frames.size() <= 1 || m_fps <= 0.0f) {
    return;
  }
  m_anim_time += dt_seconds;
  const float frame_dur = 1.0f / m_fps;
  while (m_anim_time >= frame_dur) {
    m_anim_time -= frame_dur;
    m_frame_index = (m_frame_index + 1) % static_cast<int>(m_frames.size());
  }
}

void ObjMapSpriteObject::draw(GraphicContext& gc)
{
  if (!to_draw()) {
    return;
  }
  const Sprite& spr = current_sprite();
  if (!spr.empty()) {
    spr.draw(m_pos.x, m_pos.y, gc);
  }
  if (is_selected()) {
    const Rectf r = get_bound_rect();
    gc.draw_rect(r, Color(255, 255, 0, 220));
  }
}

Rectf ObjMapSpriteObject::get_bound_rect() const
{
  const Sprite& spr = current_sprite();
  if (spr.empty()) {
    return Rectf(m_pos.x, m_pos.y, m_pos.x + 32, m_pos.y + 32);
  }
  return Rectf(m_pos.x, m_pos.y,
               m_pos.x + static_cast<float>(spr.width()),
               m_pos.y + static_cast<float>(spr.height()));
}

} // namespace flexlay
