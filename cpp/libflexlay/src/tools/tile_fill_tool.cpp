// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/tile_fill_tool.hpp"

#include "flexlay/commands/paint_command.hpp"

#include <cmath>

namespace flexlay {

void TileFillTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  if (button != 1 || !m_layer || m_layer->is_hidden()) {
    return;
  }
  const int ts = m_layer->tileset().get_tile_size();
  const int tx = static_cast<int>(std::floor(world_pos.x / static_cast<float>(ts)));
  const int ty = static_cast<int>(std::floor(world_pos.y / static_cast<float>(ts)));

  auto cmd = std::make_shared<PaintCommand>(m_layer);
  cmd->flood_fill(tx, ty, m_brush);
  if (cmd->empty()) {
    return;
  }
  if (auto* map = workspace.map()) {
    map->execute(cmd);
  }
}

} // namespace flexlay
