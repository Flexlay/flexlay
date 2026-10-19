// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_PIXEL_BUFFER_HPP
#define FLEXLAY_PIXEL_BUFFER_HPP

#include "flexlay/color.hpp"
#include "flexlay/math/size.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace flexlay {

/** RGBA8 pixel storage (row-major). Core is Qt-free; from_file needs Qt. */
class PixelBuffer
{
public:
  PixelBuffer() = default;

  static PixelBuffer from_size(const Size& size);
  static PixelBuffer from_color(const Size& size, const Color& color);
  /** Load image from path. Requires FLEXLAY_HAS_QT. Throws on failure. */
  static PixelBuffer from_file(const std::string& filename);
  /**
   * Load image and crop to [x, y, w, h]. Out-of-bounds areas are transparent.
   * Requires FLEXLAY_HAS_QT.
   */
  static PixelBuffer subregion_from_file(const std::string& filename,
                                         int x, int y, int w, int h);

  int width() const { return m_width; }
  int height() const { return m_height; }
  bool empty() const { return m_width <= 0 || m_height <= 0; }

  /** RGBA bytes, size width*height*4. */
  const std::vector<uint8_t>& data() const { return m_data; }
  std::vector<uint8_t>& data() { return m_data; }

  Color pixel(int x, int y) const;
  void set_pixel(int x, int y, const Color& c);

  /**
   * Copy a rectangular region. Coordinates outside this buffer yield
   * transparent pixels. Returns empty buffer if w or h is non-positive.
   */
  PixelBuffer subregion(int x, int y, int w, int h) const;

private:
  PixelBuffer(int width, int height, std::vector<uint8_t> data);

  int m_width = 0;
  int m_height = 0;
  std::vector<uint8_t> m_data;
};

using PixelBufferPtr = std::shared_ptr<PixelBuffer>;

} // namespace flexlay

#endif
