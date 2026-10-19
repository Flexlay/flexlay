// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/pixel_buffer.hpp"

#include <algorithm>
#include <stdexcept>

#if defined(FLEXLAY_HAS_QT)
#include <QImage>
#endif

namespace flexlay {

PixelBuffer::PixelBuffer(int width, int height, std::vector<uint8_t> data)
  : m_width(width)
  , m_height(height)
  , m_data(std::move(data))
{
}

PixelBuffer PixelBuffer::from_size(const Size& size)
{
  if (size.width < 0 || size.height < 0) {
    throw std::invalid_argument("PixelBuffer::from_size: negative size");
  }
  const size_t n = static_cast<size_t>(size.width * size.height * 4);
  return PixelBuffer(size.width, size.height, std::vector<uint8_t>(n, 0));
}

PixelBuffer PixelBuffer::from_color(const Size& size, const Color& color)
{
  PixelBuffer pb = from_size(size);
  for (int y = 0; y < pb.m_height; ++y) {
    for (int x = 0; x < pb.m_width; ++x) {
      pb.set_pixel(x, y, color);
    }
  }
  return pb;
}

PixelBuffer PixelBuffer::from_file(const std::string& filename)
{
#if defined(FLEXLAY_HAS_QT)
  QImage img(QString::fromStdString(filename));
  if (img.isNull()) {
    throw std::runtime_error("PixelBuffer::from_file: failed to load " + filename);
  }
  img = img.convertToFormat(QImage::Format_RGBA8888);
  const int w = img.width();
  const int h = img.height();
  std::vector<uint8_t> data(static_cast<size_t>(w * h * 4));
  for (int y = 0; y < h; ++y) {
    const uchar* line = img.constScanLine(y);
    std::copy(line, line + w * 4, data.begin() + static_cast<size_t>(y * w * 4));
  }
  return PixelBuffer(w, h, std::move(data));
#else
  (void)filename;
  throw std::runtime_error("PixelBuffer::from_file: built without Qt");
#endif
}

Color PixelBuffer::pixel(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
    return Color(0, 0, 0, 0);
  }
  const size_t i = static_cast<size_t>((y * m_width + x) * 4);
  return Color(m_data[i], m_data[i + 1], m_data[i + 2], m_data[i + 3]);
}

void PixelBuffer::set_pixel(int x, int y, const Color& c)
{
  if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
    return;
  }
  const size_t i = static_cast<size_t>((y * m_width + x) * 4);
  m_data[i] = c.r;
  m_data[i + 1] = c.g;
  m_data[i + 2] = c.b;
  m_data[i + 3] = c.a;
}

} // namespace flexlay
