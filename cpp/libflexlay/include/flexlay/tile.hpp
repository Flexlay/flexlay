// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TILE_HPP
#define FLEXLAY_TILE_HPP

#include "flexlay/color.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/sprite.hpp"

#include <optional>
#include <memory>
#include <string>

namespace flexlay {

class Tile
{
public:
  Tile() = default;

  explicit Tile(const Color& color) : m_color(color) {}

  Tile(const Color& color, const std::string& filename)
    : m_color(color)
    , m_filename(filename)
  {
  }

  Tile(PixelBuffer buffer)
    : m_pixelbuffer(std::make_shared<PixelBuffer>(std::move(buffer)))
  {
    if (m_pixelbuffer && !m_pixelbuffer->empty()) {
      // average color for solid-color fallback / tools
      m_color = m_pixelbuffer->pixel(0, 0);
    }
  }

  Color get_color() const { return m_color; }
  void set_color(const Color& color) { m_color = color; }

  Color get_attribute_color() const { return m_attribute_color; }
  void set_attribute_color(const Color& color) { m_attribute_color = color; }

  const std::optional<std::string>& get_filename() const { return m_filename; }
  void set_filename(const std::string& filename) { m_filename = filename; }

  bool is_transparent() const { return m_transparent; }
  void set_transparent(bool t) { m_transparent = t; }

  bool has_image() const { return m_pixelbuffer && !m_pixelbuffer->empty(); }

  std::shared_ptr<PixelBuffer> pixelbuffer() const { return m_pixelbuffer; }
  void set_pixelbuffer(std::shared_ptr<PixelBuffer> pb) { m_pixelbuffer = std::move(pb); m_sprite.reset(); }

  /** Lazy Sprite for drawing. */
  Sprite& get_sprite()
  {
    if (!m_sprite) {
      if (m_pixelbuffer) {
        m_sprite = Sprite(m_pixelbuffer);
      } else {
        m_sprite = Sprite();
      }
    }
    return *m_sprite;
  }

private:
  Color m_color{255, 255, 255, 255};
  Color m_attribute_color{255, 255, 255, 255};
  std::optional<std::string> m_filename;
  bool m_transparent = false;
  std::shared_ptr<PixelBuffer> m_pixelbuffer;
  std::optional<Sprite> m_sprite;
};

} // namespace flexlay

#endif
