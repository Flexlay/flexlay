// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/objmap_select_tool.hpp"

#include "flexlay/commands/object_delete_command.hpp"
#include "flexlay/commands/object_transform_command.hpp"

namespace flexlay {

void ObjMapSelectTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  if (button != 1 || !m_layer) {
    return;
  }

  // Prefer resize handle of currently selected rects
  for (auto& obj : m_layer->selected_objects()) {
    auto rect = std::dynamic_pointer_cast<ObjMapRectObject>(obj);
    if (!rect) {
      continue;
    }
    const ResizeHandle h = rect->hit_handle(world_pos);
    if (h != ResizeHandle::none) {
      m_mode = Mode::resizing;
      m_resize_obj = rect;
      m_resize_handle = h;
      m_resize_old_pos = rect->get_pos();
      m_resize_old_size = rect->get_size();
      m_drag_start = world_pos;
      return;
    }
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
    m_mode = Mode::moving;
    m_drag_start = world_pos;
    m_move_cmd = std::make_shared<ObjectMoveCommand>();
    for (auto& obj : selected) {
      m_move_cmd->add_obj(obj);
    }
  } else {
    m_mode = Mode::idle;
  }

  if (on_selection_changed) {
    on_selection_changed();
  }
  if (auto* map = workspace.map()) {
    map->bump_serial();
  }
}

void ObjMapSelectTool::on_mouse_move(Workspace& /*workspace*/, const Pointf& world_pos, int buttons)
{
  if (!(buttons & 0x1)) {
    return;
  }
  if (m_mode == Mode::resizing && m_resize_obj) {
    m_resize_obj->resize_to_handle(m_resize_handle, world_pos);
    return;
  }
  if (m_mode == Mode::moving && m_move_cmd) {
    const Pointf delta = world_pos - m_drag_start;
    m_move_cmd->move_by(delta);
  }
}

void ObjMapSelectTool::on_mouse_up(Workspace& workspace, const Pointf& /*world_pos*/, int button)
{
  if (button != 1) {
    return;
  }

  if (m_mode == Mode::resizing && m_resize_obj) {
    auto cmd = std::make_shared<ObjectTransformCommand>(
      m_resize_obj, m_resize_old_pos, m_resize_old_size,
      m_resize_obj->get_pos(), m_resize_obj->get_size());
    if (cmd->changed()) {
      if (auto* map = workspace.map()) {
        map->execute(cmd);
      }
    }
    m_resize_obj.reset();
    m_resize_handle = ResizeHandle::none;
  } else if (m_mode == Mode::moving && m_move_cmd && !m_move_cmd->empty()) {
    if (auto* map = workspace.map()) {
      map->execute(m_move_cmd);
    }
  }

  m_move_cmd.reset();
  m_mode = Mode::idle;
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
  if (on_selection_changed) {
    on_selection_changed();
  }
  return true;
}

} // namespace flexlay
