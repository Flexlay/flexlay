// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_transform_command.hpp"

#include <stdexcept>

namespace flexlay {

ObjectTransformCommand::ObjectTransformCommand(std::shared_ptr<ObjMapRectObject> obj,
                                               const Pointf& old_pos, const Sizef& old_size,
                                               const Pointf& new_pos, const Sizef& new_size)
  : m_obj(std::move(obj))
  , m_old_pos(old_pos)
  , m_old_size(old_size)
  , m_new_pos(new_pos)
  , m_new_size(new_size)
{
  if (!m_obj) {
    throw std::invalid_argument("ObjectTransformCommand: null object");
  }
}

void ObjectTransformCommand::execute()
{
  m_obj->set_pos(m_new_pos);
  m_obj->set_size(m_new_size);
}

void ObjectTransformCommand::undo()
{
  m_obj->set_pos(m_old_pos);
  m_obj->set_size(m_old_size);
}

} // namespace flexlay
