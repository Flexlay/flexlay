// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_move_command.hpp"

namespace flexlay {

void ObjectMoveCommand::add_obj(std::shared_ptr<ObjMapObject> obj)
{
  if (!obj) {
    return;
  }
  const Pointf p = obj->get_pos();
  m_entries.push_back(Entry{std::move(obj), p, p});
}

void ObjectMoveCommand::move_by(const Pointf& delta)
{
  for (auto& e : m_entries) {
    e.new_pos = e.old_pos + delta;
    e.obj->set_pos(e.new_pos);
  }
}

void ObjectMoveCommand::undo()
{
  for (auto& e : m_entries) {
    e.obj->set_pos(e.old_pos);
  }
}

void ObjectMoveCommand::redo()
{
  for (auto& e : m_entries) {
    e.obj->set_pos(e.new_pos);
  }
}

} // namespace flexlay
