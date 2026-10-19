// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SPRITE_HPP
#define FLEXLAY_SPRITE_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/math/origin.hpp"
#include "flexlay/pixel_buffer.hpp"

#include <memory>
#include <string>
#include <utility>

namespace flexlay {

class Sprite
{
public:
  Sprite() = default;
  explicit Sprite(PixelBuffer buffer, std::string filename = "<memory>")
    : m_buffer(std::make_shared<PixelBuffer>(std::move(buffer)))
    , m_filename(std::move(filename))
  {
  }

  explicit Sprite(std::shared_ptr<PixelBuffer> buffer, std::string filename = "<memory>")
    : m_buffer(std::move(buffer))
    , m_filename(std::move(filename))
  {
  }

  static Sprite from_file(const std::string& filename)
  {
    return Sprite(PixelBuffer::from_file(filename), filename);
  }

  bool empty() const { return !m_buffer || m_buffer->empty(); }

  int width() const { return m_buffer ? m_buffer->width() : 0; }
  int height() const { return m_buffer ? m_buffer->height() : 0; }

  const PixelBuffer& buffer() const { return *m_buffer; }
  std::shared_ptr<PixelBuffer> buffer_ptr() const { return m_buffer; }

  void set_scale(float x, float y) { m_scale = {x, y}; }
  std::pair<float, float> get_scale() const { return m_scale; }

  void set_origin(Origin origin) { m_origin = origin; }
  Origin get_origin() const { return m_origin; }

  void draw(float x, float y, GraphicContext& gc) const;

  const std::string& filename() const { return m_filename; }

private:
  std::shared_ptr<PixelBuffer> m_buffer;
  std::string m_filename;
  std::pair<float, float> m_scale{1.0f, 1.0f};
  Origin m_origin = Origin::top_left;
};

} // namespace flexlay

#endif
