// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJMAP_SPRITE_OBJECT_HPP
#define FLEXLAY_OBJMAP_SPRITE_OBJECT_HPP

#include "flexlay/objmap_object.hpp"
#include "flexlay/sprite.hpp"

#include <vector>

namespace flexlay {

class ObjMapSpriteObject : public ObjMapObject
{
public:
  ObjMapSpriteObject() = default;
  ObjMapSpriteObject(Sprite sprite, const Pointf& pos);

  void draw(GraphicContext& gc) override;
  Rectf get_bound_rect() const override;

  Sprite& sprite() { return current_sprite(); }
  const Sprite& sprite() const { return current_sprite(); }
  void set_sprite(Sprite sprite);

  /** Multi-frame animation (editor preview). Empty = static single sprite. */
  void set_frames(std::vector<Sprite> frames, float fps = 15.0f);
  bool animated() const { return m_frames.size() > 1; }
  void advance_animation(float dt_seconds);

private:
  Sprite& current_sprite();
  const Sprite& current_sprite() const;

  Sprite m_sprite;
  std::vector<Sprite> m_frames;
  float m_fps = 15.0f;
  float m_anim_time = 0.0f;
  int m_frame_index = 0;
};

} // namespace flexlay

#endif
