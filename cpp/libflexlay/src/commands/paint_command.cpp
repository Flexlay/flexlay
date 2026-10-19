// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/paint_command.hpp"

#include <stdexcept>

namespace flexlay {

PaintCommand::PaintCommand(std::shared_ptr<TilemapLayer> layer)
  : m_layer(std::move(layer))
{
  if (!m_layer) {
    throw std::invalid_argument("PaintCommand: null layer");
  }
}

void PaintCommand::paint_cell(int x, int y, uint32_t tile_id)
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

} // namespace flexlay
