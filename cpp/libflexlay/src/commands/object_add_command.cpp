// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_add_command.hpp"

#include <stdexcept>

namespace flexlay {

ObjectAddCommand::ObjectAddCommand(std::shared_ptr<ObjectLayer> layer)
  : m_layer(std::move(layer))
{
  if (!m_layer) {
    throw std::invalid_argument("ObjectAddCommand: null layer");
  }
}

void ObjectAddCommand::add_object(std::shared_ptr<ObjMapObject> obj)
{
  if (obj) {
    m_objects.push_back(std::move(obj));
  }
}

void ObjectAddCommand::execute()
{
  if (m_applied) {
    m_applied = false;
    return;
  }
  for (const auto& obj : m_objects) {
    m_layer->add_object(obj);
  }
}

void ObjectAddCommand::undo()
{
  for (const auto& obj : m_objects) {
    m_layer->remove_object(obj);
  }
}

} // namespace flexlay
