// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/tile_selector_widget.hpp"

#include "flexlay/pixel_buffer.hpp"
#include "flexlay/qt_compat.hpp"
#include "flexlay/tile.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#include <algorithm>
#include <cmath>

namespace flexlay {

TileSelectorWidget::TileSelectorWidget(QWidget* parent)
  : QWidget(parent)
{
  setMouseTracking(true);
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setMinimumWidth(64);
}

void TileSelectorWidget::set_tileset(std::shared_ptr<Tileset> tileset)
{
  m_tileset = std::move(tileset);
  update_height();
  update();
}

void TileSelectorWidget::set_tiles(std::vector<int> tiles)
{
  m_tiles = std::move(tiles);
  // drop zeros from display list (empty / spacer in tilegroups)
  m_tiles.erase(std::remove(m_tiles.begin(), m_tiles.end(), 0), m_tiles.end());
  update_height();
  update();
}

void TileSelectorWidget::set_scale(float scale)
{
  m_scale = std::max(0.25f, std::min(scale, 4.0f));
  update_height();
  update();
}

void TileSelectorWidget::set_selected_tile(int id)
{
  m_selected_id = id;
  update();
}

int TileSelectorWidget::columns() const
{
  const int cs = cell_size();
  if (cs <= 0) {
    return 1;
  }
  const int cols = std::max(1, width() / cs);
  return cols;
}

int TileSelectorWidget::cell_size() const
{
  int ts = 32;
  if (m_tileset) {
    ts = m_tileset->get_tile_size();
  }
  return std::max(8, static_cast<int>(std::lround(ts * m_scale)));
}

void TileSelectorWidget::update_height()
{
  const int cols = columns();
  const int rows = m_tiles.empty() ? 0 : static_cast<int>((m_tiles.size() + cols - 1) / cols);
  const int h = rows * cell_size();
  setMinimumHeight(std::max(h, cell_size()));
  setFixedHeight(std::max(h, 1));
}

int TileSelectorWidget::index_at(const QPoint& pos) const
{
  if (m_tiles.empty()) {
    return -1;
  }
  const int cs = cell_size();
  const int cols = columns();
  const int col = pos.x() / cs;
  const int row = pos.y() / cs;
  if (col < 0 || col >= cols || row < 0) {
    return -1;
  }
  const int idx = row * cols + col;
  if (idx < 0 || idx >= static_cast<int>(m_tiles.size())) {
    return -1;
  }
  return idx;
}

void TileSelectorWidget::draw_tile_cell(QPainter& painter, int index, int col, int row, bool highlight)
{
  const int cs = cell_size();
  const QRect cell(col * cs, row * cs, cs, cs);

  painter.fillRect(cell, QColor(40, 40, 44));

  if (!m_tileset || index < 0 || index >= static_cast<int>(m_tiles.size())) {
    return;
  }
  const int tid = m_tiles[static_cast<size_t>(index)];
  if (!m_tileset->has_tile(tid)) {
    painter.fillRect(cell.adjusted(1, 1, -1, -1), QColor(80, 80, 80));
    return;
  }

  Tile& tile = m_tileset->create(tid);
  if (tile.has_image()) {
    const PixelBuffer& pb = *tile.pixelbuffer();
    QImage qimg(const_cast<uint8_t*>(pb.data().data()), pb.width(), pb.height(),
                pb.width() * 4, QImage::Format_RGBA8888);
    // copy so QImage owns data for drawImage lifetime
    QImage owned = qimg.copy();
    painter.drawImage(cell, owned);
  } else {
    painter.fillRect(cell.adjusted(1, 1, -1, -1), to_qt(tile.get_color()));
  }

  if (tid == m_selected_id) {
    painter.setPen(QPen(QColor(255, 220, 60), 2));
    painter.drawRect(cell.adjusted(1, 1, -2, -2));
  } else if (highlight) {
    painter.setPen(QPen(QColor(180, 180, 255), 1));
    painter.drawRect(cell.adjusted(0, 0, -1, -1));
  } else {
    painter.setPen(QColor(60, 60, 64));
    painter.drawRect(cell.adjusted(0, 0, -1, -1));
  }
}

void TileSelectorWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.fillRect(rect(), QColor(32, 32, 36));

  const int cols = columns();
  const int n = static_cast<int>(m_tiles.size());
  for (int i = 0; i < n; ++i) {
    const int col = i % cols;
    const int row = i / cols;
    draw_tile_cell(painter, i, col, row, i == m_hover_index);
  }

  if (m_region_select) {
    const int cs = cell_size();
    QRect r(QPoint(std::min(m_region_start.x(), m_region_end.x()) * cs,
                   std::min(m_region_start.y(), m_region_end.y()) * cs),
            QPoint((std::max(m_region_start.x(), m_region_end.x()) + 1) * cs - 1,
                   (std::max(m_region_start.y(), m_region_end.y()) + 1) * cs - 1));
    painter.setPen(QPen(QColor(100, 200, 255), 2));
    painter.fillRect(r, QColor(100, 180, 255, 60));
    painter.drawRect(r);
  }
}

void TileSelectorWidget::mousePressEvent(QMouseEvent* event)
{
  const int idx = index_at(event->pos());
  if (event->button() == Qt::LeftButton && idx >= 0) {
    m_selected_id = m_tiles[static_cast<size_t>(idx)];
    emit tile_selected(m_selected_id);
    update();
  } else if (event->button() == Qt::RightButton && idx >= 0) {
    m_region_select = true;
    const int cols = columns();
    m_region_start = QPoint(idx % cols, idx / cols);
    m_region_end = m_region_start;
    update();
  }
}

void TileSelectorWidget::mouseMoveEvent(QMouseEvent* event)
{
  const int idx = index_at(event->pos());
  if (idx != m_hover_index) {
    m_hover_index = idx;
    update();
  }
  if (m_region_select && idx >= 0) {
    const int cols = columns();
    m_region_end = QPoint(idx % cols, idx / cols);
    update();
  }
}

void TileSelectorWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton && m_region_select) {
    m_region_select = false;
    const int cols = columns();
    const int x0 = std::min(m_region_start.x(), m_region_end.x());
    const int y0 = std::min(m_region_start.y(), m_region_end.y());
    const int x1 = std::max(m_region_start.x(), m_region_end.x());
    const int y1 = std::max(m_region_start.y(), m_region_end.y());
    const int w = x1 - x0 + 1;
    const int h = y1 - y0 + 1;
    std::vector<uint32_t> ids(static_cast<size_t>(w * h), 0);
    for (int row = y0; row <= y1; ++row) {
      for (int col = x0; col <= x1; ++col) {
        const int i = row * cols + col;
        uint32_t id = 0;
        if (i >= 0 && i < static_cast<int>(m_tiles.size())) {
          id = static_cast<uint32_t>(m_tiles[static_cast<size_t>(i)]);
        }
        ids[static_cast<size_t>((row - y0) * w + (col - x0))] = id;
      }
    }
    if (w == 1 && h == 1 && !ids.empty() && ids[0] != 0) {
      m_selected_id = static_cast<int>(ids[0]);
      emit tile_selected(m_selected_id);
    }
    emit brush_selected(w, h, ids);
    update();
  }
}

void TileSelectorWidget::leaveEvent(QEvent*)
{
  m_hover_index = -1;
  update();
}

void TileSelectorWidget::resizeEvent(QResizeEvent*)
{
  update_height();
}

} // namespace flexlay
