// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILEMAP_LAYER_HPP
#define FLEXLAY_TILEMAP_LAYER_HPP

#include "flexlay/color.hpp"
#include "flexlay/field.hpp"
#include "flexlay/layer.hpp"
#include "flexlay/tileset.hpp"

#include <memory>

namespace flexlay {

class TilemapLayer : public Layer
{
public:
  TilemapLayer(std::shared_ptr<Tileset> tileset, int width, int height);

  void draw(GraphicContext& gc, const Point& pos = Point(0, 0)) override;

  Field& field() { return m_field; }
  const Field& field() const { return m_field; }

  Tileset& tileset() { return *m_tileset; }
  const Tileset& tileset() const { return *m_tileset; }
  std::shared_ptr<Tileset> tileset_ptr() const { return m_tileset; }

  bool get_draw_grid() const { return m_draw_grid; }
  void set_draw_grid(bool v) { m_draw_grid = v; }

  Color background_color() const { return m_background_color; }
  void set_background_color(const Color& c) { m_background_color = c; }

  Color foreground_color() const { return m_foreground_color; }
  void set_foreground_color(const Color& c) { m_foreground_color = c; }

  int width() const { return m_field.width(); }
  int height() const { return m_field.height(); }
  int pixel_width() const { return m_field.width() * m_tileset->get_tile_size(); }
  int pixel_height() const { return m_field.height() * m_tileset->get_tile_size(); }

private:
  std::shared_ptr<Tileset> m_tileset;
  Field m_field;
  Color m_background_color{0, 0, 0, 0};
  Color m_foreground_color{255, 255, 255, 255};
  bool m_draw_grid = false;
  bool m_draw_attribute = false;
};

} // namespace flexlay

#endif
