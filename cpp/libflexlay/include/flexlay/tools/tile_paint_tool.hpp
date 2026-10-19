// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_TILE_PAINT_TOOL_HPP
#define FLEXLAY_TOOLS_TILE_PAINT_TOOL_HPP

#include "flexlay/commands/paint_command.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/tile_brush.hpp"
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

  void set_tile_id(uint32_t id)
  {
    m_tile_id = id;
    rebuild_brush();
  }
  uint32_t tile_id() const { return m_tile_id; }

  void set_brush_size(int size)
  {
    if (size < 1) size = 1;
    if (size > 16) size = 16;
    m_brush_size = size;
    rebuild_brush();
  }
  int brush_size() const { return m_brush_size; }

  void set_brush(const TileBrush& brush)
  {
    m_brush = brush;
    m_brush_size = std::max(m_brush.width(), m_brush.height());
  }
  const TileBrush& brush() const { return m_brush; }

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;
  void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) override;
  void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) override;
  void draw(GraphicContext& gc, Workspace& workspace) override;

private:
  void rebuild_brush()
  {
    m_brush = TileBrush::filled(m_brush_size, m_brush_size, m_tile_id, true);
  }

  Point tile_at(const Pointf& world_pos) const;
  void paint_at(Workspace& workspace, const Pointf& world_pos);
  void update_selection(const Pointf& world_pos);
  void commit_selection();

  std::shared_ptr<TilemapLayer> m_layer;
  uint32_t m_tile_id = 1;
  int m_brush_size = 1;
  TileBrush m_brush = TileBrush::single_tile(1);

  bool m_painting = false;
  std::shared_ptr<PaintCommand> m_command;
  int m_last_tx = -999999;
  int m_last_ty = -999999;

  bool m_selecting = false;
  Point m_sel_start;
  Rect m_sel_rect; // tile coords, exclusive right/bottom after normalize
};

} // namespace flexlay

#endif
