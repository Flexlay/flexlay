// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_LAYER_HPP
#define FLEXLAY_LAYER_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/math/point.hpp"

#include <string>

namespace flexlay {

class Layer
{
public:
  virtual ~Layer() = default;
  virtual void draw(GraphicContext& gc, const Point& pos = Point(0, 0)) = 0;

  bool is_hidden() const { return m_hidden; }
  void set_hidden(bool hidden) { m_hidden = hidden; }

  const std::string& get_name() const { return m_name; }
  void set_name(const std::string& name) { m_name = name; }

protected:
  std::string m_name = "<no name>";
  bool m_hidden = false;
};

} // namespace flexlay

#endif
