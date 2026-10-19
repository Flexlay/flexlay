// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GRAPHIC_CONTEXT_HPP
#define FLEXLAY_GRAPHIC_CONTEXT_HPP

#include "flexlay/color.hpp"
#include "flexlay/math/rect.hpp"

namespace flexlay {

class PixelBuffer;

class GraphicContext
{
public:
  virtual ~GraphicContext() = default;

  virtual void clear(const Color& color) = 0;
  virtual void draw_rect(const Rectf& rect, const Color& color, int radius = 0) = 0;
  virtual void fill_rect(const Rectf& rect, const Color& color, int radius = 0) = 0;
  virtual void draw_line(float x1, float y1, float x2, float y2, const Color& color) = 0;

  /** Draw pixel buffer at (x,y) with destination size (w,h). Default: no-op. */
  virtual void draw_image(const PixelBuffer& /*image*/, float /*x*/, float /*y*/,
                          float /*w*/, float /*h*/)
  {
  }

  virtual void push_modelview() = 0;
  virtual void pop_modelview() = 0;
  virtual void translate(float x, float y) = 0;
  virtual void scale(float x, float y) = 0;
  virtual void rotate(float angle_degrees) = 0;

  virtual Rectf get_clip_rect() const = 0;
  virtual void set_clip_rect(const Rectf& rect) = 0;
};

} // namespace flexlay

#endif
