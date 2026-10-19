// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/tile_paint_tool.hpp"

#include <cmath>

namespace flexlay {

void TilePaintTool::paint_at(Workspace& workspace, const Pointf& world_pos)
{
  if (!m_layer || !m_command) {
    return;
  }
  const int ts = m_layer->tileset().get_tile_size();
  const int tx = static_cast<int>(std::floor(world_pos.x / static_cast<float>(ts)));
  const int ty = static_cast<int>(std::floor(world_pos.y / static_cast<float>(ts)));
  // Align stamp to brush grid so dragging doesn't shift pattern
  const int ax = (m_brush.width() > 1) ? (tx - (tx % m_brush.width())) : tx;
  const int ay = (m_brush.height() > 1) ? (ty - (ty % m_brush.height())) : ty;
  if (ax == m_last_tx && ay == m_last_ty) {
    return;
  }
  m_last_tx = ax;
  m_last_ty = ay;
  m_command->paint_brush(ax, ay, m_brush);
  if (auto* map = workspace.map()) {
    map->set_modified(true);
    map->bump_serial();
  }
}

void TilePaintTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  if (button != 1 || !m_layer) {
    return;
  }
  m_active = true;
  m_last_tx = -999999;
  m_last_ty = -999999;
  m_command = std::make_shared<PaintCommand>(m_layer);
  paint_at(workspace, world_pos);
}

void TilePaintTool::on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons)
{
  if (!m_active || !(buttons & 0x1)) {
    return;
  }
  paint_at(workspace, world_pos);
}

void TilePaintTool::on_mouse_up(Workspace& workspace, const Pointf& /*world_pos*/, int button)
{
  if (button != 1 || !m_active) {
    return;
  }
  m_active = false;
  if (m_command && !m_command->empty()) {
    if (auto* map = workspace.map()) {
      map->execute(m_command);
    }
  }
  m_command.reset();
}

} // namespace flexlay
