// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map_widget.hpp"
#include "flexlay/qt_graphic_context.hpp"
#include "flexlay/tools/objmap_select_tool.hpp"

#include <algorithm>

#include <QKeyEvent>
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

Pointf EditorMapWidget::world_pos_from_event(const QPointF& pos) const
{
  if (!m_workspace) {
    return Pointf(0, 0);
  }
  return m_workspace->state().screen2world(
    Pointf(static_cast<float>(pos.x()), static_cast<float>(pos.y())));
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
  if (m_workspace && m_workspace->tool()) {
    const int button = (event->button() == Qt::LeftButton) ? 1
                       : (event->button() == Qt::RightButton) ? 2 : 0;
    if (button) {
      m_workspace->tool()->on_mouse_down(*m_workspace, world_pos_from_event(event->position()), button);
      update();
      emit map_changed();
      event->accept();
      return;
    }
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
  if (m_workspace && m_workspace->tool()) {
    int buttons = 0;
    if (event->buttons() & Qt::LeftButton) buttons |= 1;
    if (event->buttons() & Qt::RightButton) buttons |= 2;
    m_workspace->tool()->on_mouse_move(*m_workspace, world_pos_from_event(event->position()), buttons);
    if (buttons) {
      update();
      emit map_changed();
    }
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
  if (m_workspace && m_workspace->tool()) {
    const int button = (event->button() == Qt::LeftButton) ? 1
                       : (event->button() == Qt::RightButton) ? 2 : 0;
    if (button) {
      m_workspace->tool()->on_mouse_up(*m_workspace, world_pos_from_event(event->position()), button);
      update();
      emit map_changed();
      event->accept();
      return;
    }
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
  const float new_zoom = std::clamp(m_workspace->state().get_zoom() * factor, 0.1f, 16.0f);
  m_workspace->state().set_zoom(new_zoom, Pointf(static_cast<float>(event->position().x()),
                                                   static_cast<float>(event->position().y())));
  update();
  event->accept();
}

void EditorMapWidget::keyPressEvent(QKeyEvent* event)
{
  if (!m_workspace || !m_workspace->map()) {
    QWidget::keyPressEvent(event);
    return;
  }
  const bool ctrl = event->modifiers() & Qt::ControlModifier;
  if (ctrl && event->key() == Qt::Key_Z) {
    if (event->modifiers() & Qt::ShiftModifier)
      m_workspace->map()->redo();
    else
      m_workspace->map()->undo();
    update();
    emit map_changed();
    event->accept();
    return;
  }
  if (ctrl && event->key() == Qt::Key_Y) {
    m_workspace->map()->redo();
    update();
    emit map_changed();
    event->accept();
    return;
  }

  if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    if (auto select = std::dynamic_pointer_cast<ObjMapSelectTool>(m_workspace->tool())) {
      if (select->delete_selection(*m_workspace)) {
        update();
        emit map_changed();
        event->accept();
        return;
      }
    }
  }

  // Raise / lower selection (Python button-panel parity)
  if (auto select = std::dynamic_pointer_cast<ObjMapSelectTool>(m_workspace->tool())) {
    if (auto layer = select->object_layer()) {
      const auto sel = layer->selected_objects();
      if (!sel.empty()) {
        bool changed = false;
        if (event->key() == Qt::Key_PageUp || event->key() == Qt::Key_BracketRight) {
          for (const auto& obj : sel) {
            if (event->modifiers() & Qt::ControlModifier) {
              layer->raise_object_to_top(obj);
            } else {
              layer->raise_object(obj);
            }
          }
          changed = true;
        } else if (event->key() == Qt::Key_PageDown || event->key() == Qt::Key_BracketLeft) {
          for (const auto& obj : sel) {
            if (event->modifiers() & Qt::ControlModifier) {
              layer->lower_object_to_bottom(obj);
            } else {
              layer->lower_object(obj);
            }
          }
          changed = true;
        }
        if (changed) {
          if (auto* map = m_workspace->map()) {
            map->set_modified(true);
            map->bump_serial();
          }
          update();
          emit map_changed();
          event->accept();
          return;
        }
      }
    }
  }

  QWidget::keyPressEvent(event);
}

} // namespace flexlay
