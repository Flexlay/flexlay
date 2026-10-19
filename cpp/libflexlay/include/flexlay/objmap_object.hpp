// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJMAP_OBJECT_HPP
#define FLEXLAY_OBJMAP_OBJECT_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"

#include <string>

namespace flexlay {

class ObjMapObject
{
public:
  virtual ~ObjMapObject() = default;

  virtual void draw(GraphicContext& gc) = 0;
  virtual Rectf get_bound_rect() const = 0;
  virtual bool is_inside(const Pointf& pos) const;

  Pointf get_pos() const { return m_pos; }
  virtual void set_pos(const Pointf& pos) { m_pos = pos; }

  bool is_selected() const { return m_selected; }
  void set_selected(bool s) { m_selected = s; }

  bool to_draw() const { return m_to_draw; }
  void set_to_draw(bool v) { m_to_draw = v; }

  const std::string& name() const { return m_name; }
  void set_name(const std::string& n) { m_name = n; }

protected:
  Pointf m_pos;
  bool m_selected = false;
  bool m_to_draw = true;
  std::string m_name;
};

inline bool ObjMapObject::is_inside(const Pointf& pos) const
{
  return get_bound_rect().is_inside(pos);
}

} // namespace flexlay

#endif
