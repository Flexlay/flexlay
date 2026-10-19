// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJMAP_RECT_OBJECT_HPP
#define FLEXLAY_OBJMAP_RECT_OBJECT_HPP

#include "flexlay/color.hpp"
#include "flexlay/math/size.hpp"
#include "flexlay/objmap_object.hpp"

namespace flexlay {

class ObjMapRectObject : public ObjMapObject
{
public:
  ObjMapRectObject() = default;
  ObjMapRectObject(const Pointf& pos, const Sizef& size, const Color& color);

  void draw(GraphicContext& gc) override;
  Rectf get_bound_rect() const override;

  Sizef get_size() const { return m_size; }
  void set_size(const Sizef& size) { m_size = size; }

  Color get_color() const { return m_color; }
  void set_color(const Color& c) { m_color = c; }

private:
  Sizef m_size{32, 32};
  Color m_color{255, 255, 0, 128};
};

} // namespace flexlay

#endif
