// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_EDITOR_MAP_WIDGET_HPP
#define FLEXLAY_EDITOR_MAP_WIDGET_HPP

#include "flexlay/workspace.hpp"

#include <QWidget>
#include <memory>

namespace flexlay {

class EditorMapWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EditorMapWidget(QWidget* parent = nullptr);

  void set_workspace(std::shared_ptr<Workspace> workspace);
  std::shared_ptr<Workspace> workspace() const { return m_workspace; }

  QSize sizeHint() const override { return QSize(960, 600); }

signals:
  void map_changed();

protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

private:
  Pointf world_pos_from_event(const QPointF& pos) const;

  std::shared_ptr<Workspace> m_workspace;
  bool m_panning = false;
  QPoint m_last_mouse;
};

} // namespace flexlay

#endif
