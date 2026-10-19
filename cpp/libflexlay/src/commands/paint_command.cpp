// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/paint_command.hpp"

#include <stdexcept>
#include <vector>

namespace flexlay {

PaintCommand::PaintCommand(std::shared_ptr<TilemapLayer> layer)
  : m_layer(std::move(layer))
{
  if (!m_layer) {
    throw std::invalid_argument("PaintCommand: null layer");
  }
}

void PaintCommand::record_cell(int x, int y, uint32_t tile_id)
{
  Field& field = m_layer->field();
  if (x < 0 || y < 0 || x >= field.width() || y >= field.height()) {
    return;
  }

  for (auto& c : m_changes) {
    if (c.x == x && c.y == y) {
      if (c.new_id == tile_id) {
        return;
      }
      field.put(x, y, tile_id);
      c.new_id = tile_id;
      m_applied = true;
      return;
    }
  }

  const uint32_t old_id = field.at(x, y);
  if (old_id == tile_id) {
    return;
  }
  field.put(x, y, tile_id);
  m_changes.push_back(CellChange{x, y, old_id, tile_id});
  m_applied = true;
}

void PaintCommand::paint_cell(int x, int y, uint32_t tile_id)
{
  record_cell(x, y, tile_id);
}

void PaintCommand::paint_brush(int x, int y, const TileBrush& brush)
{
  for (int by = 0; by < brush.height(); ++by) {
    for (int bx = 0; bx < brush.width(); ++bx) {
      const uint32_t id = brush.at(bx, by);
      if (!brush.is_opaque() && id == 0) {
        continue;
      }
      record_cell(x + bx, y + by, id);
    }
  }
}

void PaintCommand::execute()
{
  if (m_applied) {
    m_applied = false;
    return;
  }
  for (const auto& c : m_changes) {
    m_layer->field().put(c.x, c.y, c.new_id);
  }
}

void PaintCommand::undo()
{
  for (auto it = m_changes.rbegin(); it != m_changes.rend(); ++it) {
    m_layer->field().put(it->x, it->y, it->old_id);
  }
}

void PaintCommand::redo()
{
  for (const auto& c : m_changes) {
    m_layer->field().put(c.x, c.y, c.new_id);
  }
}

void PaintCommand::flood_fill(int x, int y, const TileBrush& brush)
{
  Field& field = m_layer->field();
  if (x < 0 || y < 0 || x >= field.width() || y >= field.height()) {
    return;
  }
  if (brush.width() < 1 || brush.height() < 1) {
    return;
  }

  const uint32_t replace_id = field.at(x, y);
  // Avoid infinite fill if every brush cell is replace_id
  bool brush_has_other = false;
  for (int by = 0; by < brush.height() && !brush_has_other; ++by) {
    for (int bx = 0; bx < brush.width(); ++bx) {
      if (brush.at(bx, by) != replace_id) {
        brush_has_other = true;
        break;
      }
    }
  }
  if (!brush_has_other) {
    return;
  }

  const int w = field.width();
  const int h = field.height();
  std::vector<uint8_t> visited(static_cast<size_t>(w * h), 0);
  std::vector<std::pair<int, int>> stack;
  stack.emplace_back(x, y);

  auto idx = [w](int px, int py) { return static_cast<size_t>(py * w + px); };

  while (!stack.empty()) {
    const auto [cx, cy] = stack.back();
    stack.pop_back();
    if (cx < 0 || cy < 0 || cx >= w || cy >= h) {
      continue;
    }
    if (visited[idx(cx, cy)]) {
      continue;
    }
    if (field.at(cx, cy) != replace_id) {
      continue;
    }
    visited[idx(cx, cy)] = 1;

    int bx = (cx - x) % brush.width();
    int by = (cy - y) % brush.height();
    if (bx < 0) bx += brush.width();
    if (by < 0) by += brush.height();
    const uint32_t fill_id = brush.at(bx, by);
    record_cell(cx, cy, fill_id);

    stack.emplace_back(cx + 1, cy);
    stack.emplace_back(cx - 1, cy);
    stack.emplace_back(cx, cy + 1);
    stack.emplace_back(cx, cy - 1);
  }
}

} // namespace flexlay
