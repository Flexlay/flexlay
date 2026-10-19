// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJMAP_SPRITE_OBJECT_HPP
#define FLEXLAY_OBJMAP_SPRITE_OBJECT_HPP

#include "flexlay/objmap_object.hpp"
#include "flexlay/sprite.hpp"

namespace flexlay {

class ObjMapSpriteObject : public ObjMapObject
{
public:
  ObjMapSpriteObject() = default;
  ObjMapSpriteObject(Sprite sprite, const Pointf& pos);

  void draw(GraphicContext& gc) override;
  Rectf get_bound_rect() const override;

  Sprite& sprite() { return m_sprite; }
  const Sprite& sprite() const { return m_sprite; }
  void set_sprite(Sprite sprite) { m_sprite = std::move(sprite); }

private:
  Sprite m_sprite;
};

} // namespace flexlay

#endif
