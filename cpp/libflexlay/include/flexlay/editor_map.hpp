// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_EDITOR_MAP_HPP
#define FLEXLAY_EDITOR_MAP_HPP

#include "flexlay/color.hpp"
#include "flexlay/graphic_context.hpp"
#include "flexlay/layer.hpp"
#include "flexlay/math/rect.hpp"

#include <memory>
#include <vector>

namespace flexlay {

class EditorMap
{
public:
  EditorMap() = default;

  void add_layer(std::shared_ptr<Layer> layer, int pos = -1);
  const std::vector<std::shared_ptr<Layer>>& layers() const { return m_layers; }

  void draw(GraphicContext& gc) const;

  void set_background_color(const Color& c) { m_background_color = c; }
  Color background_color() const { return m_background_color; }

  void set_bounding_rect(const Rectf& r)
  {
    m_bounding_rect = r;
    m_has_bounding_rect = true;
  }

  bool has_bounding_rect() const { return m_has_bounding_rect; }
  Rectf bounding_rect() const { return m_bounding_rect; }

  void set_draw_grid(bool v) { m_draw_grid = v; }
  bool draw_grid() const { return m_draw_grid; }

  int serial() const { return m_serial; }
  void bump_serial() { ++m_serial; }

  bool is_modified() const { return m_modified; }
  void set_modified(bool m) { m_modified = m; }

private:
  Color m_background_color{100, 80, 100};
  bool m_modified = false;
  int m_serial = 0;
  bool m_has_bounding_rect = false;
  Rectf m_bounding_rect;
  std::vector<std::shared_ptr<Layer>> m_layers;
  bool m_draw_grid = false;
};

} // namespace flexlay

#endif
