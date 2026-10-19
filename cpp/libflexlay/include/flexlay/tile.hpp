// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILE_HPP
#define FLEXLAY_TILE_HPP

#include "flexlay/color.hpp"

#include <optional>
#include <string>

namespace flexlay {

class Tile
{
public:
  Tile() = default;
  explicit Tile(const Color& color) : m_color(color) {}
  Tile(const Color& color, const std::string& filename)
    : m_color(color), m_filename(filename) {}

  Color get_color() const { return m_color; }
  void set_color(const Color& color) { m_color = color; }

  Color get_attribute_color() const { return m_attribute_color; }
  void set_attribute_color(const Color& color) { m_attribute_color = color; }

  const std::optional<std::string>& get_filename() const { return m_filename; }
  void set_filename(const std::string& filename) { m_filename = filename; }

  bool is_transparent() const { return m_transparent; }
  void set_transparent(bool t) { m_transparent = t; }

private:
  Color m_color{255, 255, 255, 255};
  Color m_attribute_color{255, 255, 255, 255};
  std::optional<std::string> m_filename;
  bool m_transparent = false;
};

} // namespace flexlay

#endif
