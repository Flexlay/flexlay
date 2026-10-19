// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_MINIMAP_WIDGET_HPP
#define FLEXLAY_GUI_MINIMAP_WIDGET_HPP

#include "flexlay/workspace.hpp"

#include <QWidget>

#include <memory>

namespace flexlay {

/** Simple overview of the current map; click/drag to pan the main view. */
class MinimapWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MinimapWidget(QWidget* parent = nullptr);

  void set_workspace(std::shared_ptr<Workspace> workspace);
  QSize sizeHint() const override { return QSize(280, 160); }
  QSize minimumSizeHint() const override { return QSize(120, 80); }

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  void pan_to(const QPoint& widget_pos);

  std::shared_ptr<Workspace> m_workspace;
  bool m_drag = false;
};

} // namespace flexlay

#endif
