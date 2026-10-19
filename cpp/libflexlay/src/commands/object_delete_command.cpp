// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_delete_command.hpp"

#include <stdexcept>

namespace flexlay {

ObjectDeleteCommand::ObjectDeleteCommand(std::shared_ptr<ObjectLayer> layer)
  : m_layer(std::move(layer))
{
  if (!m_layer) {
    throw std::invalid_argument("ObjectDeleteCommand: null layer");
  }
}

void ObjectDeleteCommand::add_object(std::shared_ptr<ObjMapObject> obj)
{
  if (obj) {
    m_objects.push_back(std::move(obj));
  }
}

void ObjectDeleteCommand::execute()
{
  for (const auto& obj : m_objects) {
    m_layer->remove_object(obj);
  }
}

void ObjectDeleteCommand::undo()
{
  for (const auto& obj : m_objects) {
    m_layer->add_object(obj);
  }
}

} // namespace flexlay
