// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/minimap_widget.hpp"

#include "flexlay/editor_map.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

#include <algorithm>
#include <cmath>

namespace flexlay {

MinimapWidget::MinimapWidget(QWidget* parent)
  : QWidget(parent)
{
  setAutoFillBackground(true);
}

void MinimapWidget::set_workspace(std::shared_ptr<Workspace> workspace)
{
  m_workspace = std::move(workspace);
  update();
}

void MinimapWidget::paintEvent(QPaintEvent* /*event*/)
{
  QPainter p(this);
  p.fillRect(rect(), QColor(40, 45, 55));

  if (!m_workspace || !m_workspace->map()) {
    p.setPen(QColor(180, 180, 180));
    p.drawText(rect(), Qt::AlignCenter, tr("No map"));
    return;
  }

  EditorMap* map = m_workspace->map();
  const Rectf br = map->bounding_rect();
  const float mw = std::max(1.0f, br.width());
  const float mh = std::max(1.0f, br.height());
  const float scale = std::min(static_cast<float>(width()) / mw,
                               static_cast<float>(height()) / mh);
  const float ox = (static_cast<float>(width()) - mw * scale) * 0.5f;
  const float oy = (static_cast<float>(height()) - mh * scale) * 0.5f;

  auto bg = map->background_color();
  p.fillRect(QRectF(ox, oy, mw * scale, mh * scale),
             QColor(bg.r, bg.g, bg.b, 255));

  // Sample solid-ish tilemaps into the overview
  for (const auto& layer : map->layers()) {
    if (!layer || layer->is_hidden()) {
      continue;
    }
    auto* tm = dynamic_cast<TilemapLayer*>(layer.get());
    if (!tm || tm->width() <= 0 || tm->height() <= 0) {
      continue;
    }
    const int ts = tm->tileset().get_tile_size();
    const int step = std::max(1, std::max(tm->width(), tm->height()) / 64);
    for (int y = 0; y < tm->height(); y += step) {
      for (int x = 0; x < tm->width(); x += step) {
        const uint32_t id = tm->field().at(x, y);
        if (id == 0) {
          continue;
        }
        Color c(80, 80, 90);
        if (tm->tileset().has_tile(static_cast<int>(id))) {
          c = tm->tileset().create(static_cast<int>(id)).get_color();
        }
        const float px = ox + (static_cast<float>(x * ts) - br.left) * scale;
        const float py = oy + (static_cast<float>(y * ts) - br.top) * scale;
        const float pw = std::max(1.0f, static_cast<float>(ts * step) * scale);
        const float ph = std::max(1.0f, static_cast<float>(ts * step) * scale);
        p.fillRect(QRectF(px, py, pw, ph), QColor(c.r, c.g, c.b, 220));
      }
    }
  }

  // Objects as dots
  for (const auto& layer : map->layers()) {
    auto* ol = dynamic_cast<ObjectLayer*>(layer.get());
    if (!ol) {
      continue;
    }
    p.setBrush(QColor(255, 220, 80));
    p.setPen(Qt::NoPen);
    for (const auto& obj : ol->objects()) {
      if (!obj) {
        continue;
      }
      const auto pos = obj->get_pos();
      const float px = ox + (pos.x - br.left) * scale;
      const float py = oy + (pos.y - br.top) * scale;
      p.drawEllipse(QPointF(px, py), 2.5, 2.5);
    }
  }

  // Viewport rectangle from current camera clip
  {
    const Rectf clip = m_workspace->state().get_clip_rect();
    const float vx = ox + (clip.left - br.left) * scale;
    const float vy = oy + (clip.top - br.top) * scale;
    const float vw = std::max(1.0f, clip.width()) * scale;
    const float vh = std::max(1.0f, clip.height()) * scale;
    p.setBrush(QColor(255, 255, 0, 40));
    p.setPen(QPen(QColor(255, 255, 0), 1));
    p.drawRect(QRectF(vx, vy, vw, vh));
  }
}

void MinimapWidget::pan_to(const QPoint& widget_pos)
{
  if (!m_workspace || !m_workspace->map()) {
    return;
  }
  const Rectf br = m_workspace->map()->bounding_rect();
  const float mw = std::max(1.0f, br.width());
  const float mh = std::max(1.0f, br.height());
  const float scale = std::min(static_cast<float>(width()) / mw,
                               static_cast<float>(height()) / mh);
  const float ox = (static_cast<float>(width()) - mw * scale) * 0.5f;
  const float oy = (static_cast<float>(height()) - mh * scale) * 0.5f;
  const float wx = br.left + (static_cast<float>(widget_pos.x()) - ox) / scale;
  const float wy = br.top + (static_cast<float>(widget_pos.y()) - oy) / scale;
  m_workspace->state().set_pos(Pointf(wx, wy));
  update();
}

void MinimapWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    m_drag = true;
    pan_to(event->pos());
    event->accept();
  }
}

void MinimapWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (m_drag) {
    pan_to(event->pos());
    event->accept();
  }
}

void MinimapWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    m_drag = false;
    event->accept();
  }
}

} // namespace flexlay
