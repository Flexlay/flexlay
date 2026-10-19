// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/objmap_select_tool.hpp"

#include "flexlay/commands/object_delete_command.hpp"

namespace flexlay {

void ObjMapSelectTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  if (button != 1 || !m_layer) {
    return;
  }

  auto hit = m_layer->find_object(world_pos);
  if (hit) {
    if (!hit->is_selected()) {
      m_layer->clear_selection();
      hit->set_selected(true);
    }
  } else {
    m_layer->clear_selection();
  }

  auto selected = m_layer->selected_objects();
  if (!selected.empty()) {
    m_dragging = true;
    m_drag_start = world_pos;
    m_move_cmd = std::make_shared<ObjectMoveCommand>();
    for (auto& obj : selected) {
      m_move_cmd->add_obj(obj);
    }
  }

  if (auto* map = workspace.map()) {
    map->bump_serial();
  }
}

void ObjMapSelectTool::on_mouse_move(Workspace& /*workspace*/, const Pointf& world_pos, int buttons)
{
  if (!m_dragging || !(buttons & 0x1) || !m_move_cmd) {
    return;
  }
  const Pointf delta = world_pos - m_drag_start;
  m_move_cmd->move_by(delta);
}

void ObjMapSelectTool::on_mouse_up(Workspace& workspace, const Pointf& /*world_pos*/, int button)
{
  if (button != 1 || !m_dragging) {
    return;
  }
  m_dragging = false;
  if (m_move_cmd && !m_move_cmd->empty()) {
    // Only commit if something actually moved
    // move_by already applied; execute() is no-op
    if (auto* map = workspace.map()) {
      map->execute(m_move_cmd);
    }
  }
  m_move_cmd.reset();
}


bool ObjMapSelectTool::delete_selection(Workspace& workspace)
{
  if (!m_layer) {
    return false;
  }
  auto selected = m_layer->selected_objects();
  if (selected.empty()) {
    return false;
  }
  auto cmd = std::make_shared<ObjectDeleteCommand>(m_layer);
  for (auto& obj : selected) {
    cmd->add_object(obj);
  }
  if (auto* map = workspace.map()) {
    map->execute(cmd);
  }
  return true;
}

} // namespace flexlay
