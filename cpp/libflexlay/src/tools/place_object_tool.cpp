// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/tools/place_object_tool.hpp"

#include "flexlay/commands/object_add_command.hpp"
#include "flexlay/color.hpp"

namespace flexlay {

void PlaceObjectTool::on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button)
{
  m_cursor = world_pos;
  m_have_cursor = true;

  if (button == 2) {
    if (on_cancelled) {
      on_cancelled();
    }
    return;
  }
  if (button != 1 || !m_layer || !m_proto) {
    return;
  }

  auto obj = m_proto->instantiate(world_pos, m_datadir);
  m_layer->clear_selection();
  auto cmd = std::make_shared<ObjectAddCommand>(m_layer);
  cmd->add_object(obj);
  if (auto* map = workspace.map()) {
    map->execute(cmd);
  }
  obj->set_selected(true);
  if (on_placed) {
    on_placed(obj);
  }
}

void PlaceObjectTool::on_mouse_move(Workspace& /*workspace*/, const Pointf& world_pos, int /*buttons*/)
{
  m_cursor = world_pos;
  m_have_cursor = true;
}

void PlaceObjectTool::on_mouse_up(Workspace& /*workspace*/, const Pointf& /*world_pos*/, int /*button*/)
{
}

void PlaceObjectTool::draw(GraphicContext& gc, Workspace& /*workspace*/)
{
  if (!m_have_cursor || !m_proto) {
    return;
  }
  const Sizef sz = m_proto->default_size;
  Color c = m_proto->default_color;
  c.a = 100;
  gc.fill_rect(Rectf(m_cursor.x, m_cursor.y, m_cursor.x + sz.width, m_cursor.y + sz.height), c);
  gc.draw_rect(Rectf(m_cursor.x, m_cursor.y, m_cursor.x + sz.width, m_cursor.y + sz.height),
               Color(255, 255, 255, 180));
}

} // namespace flexlay
