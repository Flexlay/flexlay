// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_GUI_TILE_SELECTOR_WIDGET_HPP
#define FLEXLAY_GUI_TILE_SELECTOR_WIDGET_HPP

#include "flexlay/tileset.hpp"

#include <QWidget>

#include <cstdint>
#include <memory>
#include <vector>

namespace flexlay {

/**
 * Scrollable grid of tiles from a Tileset.
 * Click selects a tile id (emitted as tile_selected).
 * Optional region drag builds a multi-tile TileBrush (emitted as brush_selected).
 */
class TileSelectorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TileSelectorWidget(QWidget* parent = nullptr);

  void set_tileset(std::shared_ptr<Tileset> tileset);
  std::shared_ptr<Tileset> tileset() const { return m_tileset; }

  /** Display these tile ids (order preserved). Empty = no tiles. */
  void set_tiles(std::vector<int> tiles);
  const std::vector<int>& tiles() const { return m_tiles; }

  void set_scale(float scale);
  float scale() const { return m_scale; }

  void set_selected_tile(int id);
  int selected_tile() const { return m_selected_id; }

signals:
  void tile_selected(int tile_id);
  /** Emitted after RMB-drag region select; brush field is opaque, zeros for empty cells. */
  void brush_selected(int width, int height, const std::vector<uint32_t>& ids);

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

private:
  int columns() const;
  int cell_size() const;
  int index_at(const QPoint& pos) const;
  void update_height();
  void draw_tile_cell(QPainter& painter, int index, int col, int row, bool highlight);

  std::shared_ptr<Tileset> m_tileset;
  std::vector<int> m_tiles;
  float m_scale = 1.0f;
  int m_selected_id = -1;
  int m_hover_index = -1;

  bool m_region_select = false;
  QPoint m_region_start; // grid cell
  QPoint m_region_end;
};

} // namespace flexlay

#endif
