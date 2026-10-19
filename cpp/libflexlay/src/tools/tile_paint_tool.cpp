// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/tile_paint_tool.hpp"

#include "flexlay/color.hpp"

#include <algorithm>
#include <cmath>

namespace flexlay {

Point TilePaintTool::tile_at(const Pointf& world_pos) const
{
  const int ts = m_layer ? m_layer->tileset().get_tile_size() : 32;
  return Point(static_cast<int>(std::floor(world_pos.x / static_cast<float>(ts))),
               static_cast<int>(std::floor(world_pos.y / static_cast<float>(ts))));
}

void TilePaintTool::update_selection(const Pointf& world_pos)
{
  const Point p = tile_at(world_pos);
  m_sel_rect = Rect(std::min(m_sel_start.x, p.x),
                    std::min(m_sel_start.y, p.y),
                    std::max(m_sel_start.x, p.x) + 1,
                    std::max(m_sel_start.y, p.y) + 1);
}

void TilePaintTool::commit_selection()
{
  if (!m_layer) {
    return;
  }
  Field& field = m_layer->field();
  Rect sel = m_sel_rect;
  // Clamp to field
  sel.left = std::max(0, sel.left);
  sel.top = std::max(0, sel.top);
  sel.right = std::min(field.width(), sel.right);
  sel.bottom = std::min(field.height(), sel.bottom);
  if (sel.right <= sel.left || sel.bottom <= sel.top) {
    return;
  }
  m_brush = TileBrush::from_field(field, sel.left, sel.top, sel.width(), sel.height());
  m_brush_size = std::max(m_brush.width(), m_brush.height());
}

void TilePaintTool::paint_at(Workspace& workspace, const Pointf& world_pos)
{
  if (!m_layer || !m_command) {
    return;
  }
  const Point t = tile_at(world_pos);
  const int ax = (m_brush.width() > 1) ? (t.x - (t.x % m_brush.width())) : t.x;
  const int ay = (m_brush.height() > 1) ? (t.y - (t.y % m_brush.height())) : t.y;
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
  if (!m_layer) {
    return;
  }
  if (button == 2) {
    // Right: start region select for brush copy
    m_selecting = true;
    m_sel_start = tile_at(world_pos);
    update_selection(world_pos);
    return;
  }
  if (button != 1) {
    return;
  }
  m_painting = true;
  m_last_tx = -999999;
  m_last_ty = -999999;
  m_command = std::make_shared<PaintCommand>(m_layer);
  paint_at(workspace, world_pos);
}

void TilePaintTool::on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons)
{
  if (m_selecting && (buttons & 0x2)) {
    update_selection(world_pos);
    return;
  }
  if (!m_painting || !(buttons & 0x1)) {
    return;
  }
  paint_at(workspace, world_pos);
}

void TilePaintTool::on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button)
{
  if (button == 2 && m_selecting) {
    update_selection(world_pos);
    commit_selection();
    m_selecting = false;
    if (auto* map = workspace.map()) {
      map->bump_serial();
    }
    return;
  }
  if (button != 1 || !m_painting) {
    return;
  }
  m_painting = false;
  if (m_command && !m_command->empty()) {
    if (auto* map = workspace.map()) {
      map->execute(m_command);
    }
  }
  m_command.reset();
}

void TilePaintTool::draw(GraphicContext& gc, Workspace& /*workspace*/)
{
  if (!m_selecting || !m_layer) {
    return;
  }
  const int ts = m_layer->tileset().get_tile_size();
  const Rectf r(static_cast<float>(m_sel_rect.left * ts),
                static_cast<float>(m_sel_rect.top * ts),
                static_cast<float>(m_sel_rect.right * ts),
                static_cast<float>(m_sel_rect.bottom * ts));
  gc.fill_rect(r, Color(255, 255, 255, 80));
  gc.draw_rect(r, Color(255, 255, 255, 200));
}

} // namespace flexlay
