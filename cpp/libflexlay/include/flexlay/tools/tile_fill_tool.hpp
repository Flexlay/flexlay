// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_TILE_FILL_TOOL_HPP
#define FLEXLAY_TOOLS_TILE_FILL_TOOL_HPP

#include "flexlay/tile_brush.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tools/tool.hpp"
#include "flexlay/workspace.hpp"

#include <memory>

namespace flexlay {

class TileFillTool : public Tool
{
public:
  TileFillTool() = default;

  void set_tilemap(std::shared_ptr<TilemapLayer> layer) { m_layer = std::move(layer); }
  void set_brush(const TileBrush& brush) { m_brush = brush; }
  void set_tile_id(uint32_t id)
  {
    m_brush = TileBrush::single_tile(id);
  }
  const TileBrush& brush() const { return m_brush; }

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;

private:
  std::shared_ptr<TilemapLayer> m_layer;
  TileBrush m_brush = TileBrush::single_tile(1);
};

} // namespace flexlay

#endif
