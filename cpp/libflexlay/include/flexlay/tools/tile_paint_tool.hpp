// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_TILE_PAINT_TOOL_HPP
#define FLEXLAY_TOOLS_TILE_PAINT_TOOL_HPP

#include "flexlay/commands/paint_command.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tools/tool.hpp"
#include "flexlay/workspace.hpp"

#include <cstdint>
#include <memory>

namespace flexlay {

class TilePaintTool : public Tool
{
public:
  TilePaintTool() = default;

  void set_tilemap(std::shared_ptr<TilemapLayer> layer) { m_layer = std::move(layer); }
  void set_tile_id(uint32_t id) { m_tile_id = id; }
  uint32_t tile_id() const { return m_tile_id; }

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;
  void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) override;
  void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) override;

private:
  void paint_at(Workspace& workspace, const Pointf& world_pos);

  std::shared_ptr<TilemapLayer> m_layer;
  uint32_t m_tile_id = 1;
  bool m_active = false;
  std::shared_ptr<PaintCommand> m_command;
};

} // namespace flexlay

#endif
