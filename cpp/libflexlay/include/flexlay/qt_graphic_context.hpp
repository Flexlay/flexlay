// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_QT_GRAPHIC_CONTEXT_HPP
#define FLEXLAY_QT_GRAPHIC_CONTEXT_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/qt_compat.hpp"

#include <QImage>
#include <QLineF>
#include <QPainter>
#include <QPainterPath>

namespace flexlay {

class QtGraphicContext : public GraphicContext
{
public:
  explicit QtGraphicContext(QPainter& painter) : m_painter(painter) {}

  void clear(const Color&) override {}

  void draw_rect(const Rectf& rect, const Color& color, int radius = 0) override
  {
    m_painter.setPen(to_qt(color));
    m_painter.setBrush(Qt::NoBrush);
    if (radius == 0) {
      m_painter.drawRect(to_qt(rect));
    } else {
      m_painter.drawRoundedRect(to_qt(rect), radius, radius);
    }
  }

  void fill_rect(const Rectf& rect, const Color& color, int radius = 0) override
  {
    if (radius == 0) {
      m_painter.fillRect(to_qt(rect), to_qt(color));
      return;
    }
    QPainterPath path;
    path.addRoundedRect(to_qt(rect), radius, radius);
    m_painter.fillPath(path, to_qt(color));
  }

  void draw_line(float x1, float y1, float x2, float y2, const Color& color) override
  {
    m_painter.setPen(to_qt(color));
    m_painter.drawLine(QLineF(x1, y1, x2, y2));
  }

  void draw_image(const PixelBuffer& image, float x, float y, float w, float h) override
  {
    if (image.empty()) {
      return;
    }
    QImage qimg(image.data().data(), image.width(), image.height(), image.width() * 4,
                QImage::Format_RGBA8888);
    // QImage does not own; keep buffer alive for this call only
    m_painter.drawImage(QRectF(x, y, w, h), qimg);
  }

  void push_modelview() override { m_painter.save(); }
  void pop_modelview() override { m_painter.restore(); }

  void translate(float x, float y) override
  {
    m_painter.setViewTransformEnabled(true);
    m_painter.translate(x, y);
  }

  void scale(float x, float y) override
  {
    m_painter.setViewTransformEnabled(true);
    m_painter.scale(x, y);
  }

  void rotate(float angle_degrees) override
  {
    m_painter.setViewTransformEnabled(true);
    m_painter.rotate(angle_degrees);
  }

  Rectf get_clip_rect() const override { return m_clip; }
  void set_clip_rect(const Rectf& rect) override { m_clip = rect; }

private:
  QPainter& m_painter;
  Rectf m_clip{0, 0, 0, 0};
};

} // namespace flexlay

#endif
