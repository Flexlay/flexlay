// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_property_command.hpp"

namespace flexlay {

ObjectPropertyCommand ObjectPropertyCommand::set_name(std::shared_ptr<ObjMapObject> obj,
                                                      std::string old_name, std::string new_name)
{
  ObjectPropertyCommand c;
  c.m_kind = Kind::Name;
  c.m_obj = std::move(obj);
  c.m_old_name = std::move(old_name);
  c.m_new_name = std::move(new_name);
  c.m_label = "Set object name";
  return c;
}

ObjectPropertyCommand ObjectPropertyCommand::set_pos(std::shared_ptr<ObjMapObject> obj,
                                                     Pointf old_pos, Pointf new_pos)
{
  ObjectPropertyCommand c;
  c.m_kind = Kind::Pos;
  c.m_obj = std::move(obj);
  c.m_old_pos = old_pos;
  c.m_new_pos = new_pos;
  c.m_label = "Set object position";
  return c;
}

ObjectPropertyCommand ObjectPropertyCommand::set_property(std::shared_ptr<ObjMapObject> obj,
                                                          std::string key,
                                                          std::optional<PropertyValue> old_value,
                                                          PropertyValue new_value)
{
  ObjectPropertyCommand c;
  c.m_kind = Kind::Prop;
  c.m_obj = std::move(obj);
  c.m_key = std::move(key);
  c.m_old_prop = std::move(old_value);
  c.m_new_prop = std::move(new_value);
  c.m_label = "Set object property";
  return c;
}

bool ObjectPropertyCommand::changed() const
{
  if (!m_obj) {
    return false;
  }
  switch (m_kind) {
    case Kind::Name:
      return m_old_name != m_new_name;
    case Kind::Pos:
      return m_old_pos.x != m_new_pos.x || m_old_pos.y != m_new_pos.y;
    case Kind::Prop:
      if (!m_old_prop) {
        return true;
      }
      return *m_old_prop != m_new_prop;
  }
  return false;
}

void ObjectPropertyCommand::execute()
{
  if (!m_obj) {
    return;
  }
  switch (m_kind) {
    case Kind::Name:
      m_obj->set_name(m_new_name);
      break;
    case Kind::Pos:
      m_obj->set_pos(m_new_pos);
      break;
    case Kind::Prop:
      m_obj->properties().set(m_key, m_new_prop);
      break;
  }
}

void ObjectPropertyCommand::undo()
{
  if (!m_obj) {
    return;
  }
  switch (m_kind) {
    case Kind::Name:
      m_obj->set_name(m_old_name);
      break;
    case Kind::Pos:
      m_obj->set_pos(m_old_pos);
      break;
    case Kind::Prop:
      if (m_old_prop) {
        m_obj->properties().set(m_key, *m_old_prop);
      } else {
        m_obj->properties().erase(m_key);
      }
      break;
  }
}

} // namespace flexlay
