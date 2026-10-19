// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map_widget.hpp"

#include "flexlay/qt_graphic_context.hpp"

#include <algorithm>

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

namespace flexlay {

EditorMapWidget::EditorMapWidget(QWidget* parent)
  : QWidget(parent)
{
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setAutoFillBackground(false);
  setAttribute(Qt::WA_OpaquePaintEvent);
}

void EditorMapWidget::set_workspace(std::shared_ptr<Workspace> workspace)
{
  m_workspace = std::move(workspace);
  if (m_workspace) {
    m_workspace->state().set_size(width(), height());
  }
  update();
}

void EditorMapWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.fillRect(rect(), QColor(32, 32, 40));

  if (!m_workspace || !m_workspace->get_map()) {
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(rect(), Qt::AlignCenter, tr("No map loaded"));
    return;
  }

  painter.setRenderHint(QPainter::Antialiasing, false);
  QtGraphicContext gc(painter);
  m_workspace->draw(gc);
}

void EditorMapWidget::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  if (m_workspace) {
    m_workspace->state().set_size(width(), height());
  }
}

void EditorMapWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MiddleButton) {
    m_panning = true;
    m_last_mouse = event->pos();
    setCursor(Qt::ClosedHandCursor);
    event->accept();
    return;
  }
  QWidget::mousePressEvent(event);
}

void EditorMapWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (m_panning && m_workspace) {
    const QPoint delta = event->pos() - m_last_mouse;
    m_last_mouse = event->pos();
    auto offset = m_workspace->state().get_offset();
    const float zoom = m_workspace->state().get_zoom();
    offset.x += static_cast<float>(delta.x()) / zoom;
    offset.y += static_cast<float>(delta.y()) / zoom;
    m_workspace->state().set_offset(offset);
    update();
    event->accept();
    return;
  }
  QWidget::mouseMoveEvent(event);
}

void EditorMapWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MiddleButton && m_panning) {
    m_panning = false;
    unsetCursor();
    event->accept();
    return;
  }
  QWidget::mouseReleaseEvent(event);
}

void EditorMapWidget::wheelEvent(QWheelEvent* event)
{
  if (!m_workspace) {
    QWidget::wheelEvent(event);
    return;
  }

  const QPoint num_degrees = event->angleDelta();
  if (num_degrees.y() == 0) {
    QWidget::wheelEvent(event);
    return;
  }

  const float factor = num_degrees.y() > 0 ? 1.1f : (1.0f / 1.1f);
  const float old_zoom = m_workspace->state().get_zoom();
  const float new_zoom = std::clamp(old_zoom * factor, 0.1f, 16.0f);
  const Pointf screen(static_cast<float>(event->position().x()),
                      static_cast<float>(event->position().y()));
  m_workspace->state().set_zoom(new_zoom, screen);
  update();
  event->accept();
}

} // namespace flexlay
