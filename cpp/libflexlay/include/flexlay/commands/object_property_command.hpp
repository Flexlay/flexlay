// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_OBJECT_PROPERTY_COMMAND_HPP
#define FLEXLAY_COMMANDS_OBJECT_PROPERTY_COMMAND_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/objmap_object.hpp"
#include "flexlay/property_bag.hpp"

#include <memory>
#include <optional>
#include <string>

namespace flexlay {

/** Undoable change to object name, position, or a single PropertyBag entry. */
class ObjectPropertyCommand : public Command
{
public:
  static ObjectPropertyCommand set_name(std::shared_ptr<ObjMapObject> obj,
                                        std::string old_name, std::string new_name);
  static ObjectPropertyCommand set_pos(std::shared_ptr<ObjMapObject> obj,
                                       Pointf old_pos, Pointf new_pos);
  static ObjectPropertyCommand set_property(std::shared_ptr<ObjMapObject> obj,
                                            std::string key,
                                            std::optional<PropertyValue> old_value,
                                            PropertyValue new_value);

  void execute() override;
  void undo() override;
  void redo() override { execute(); }
  std::string name() const override { return m_label; }

  bool changed() const;

private:
  enum class Kind { Name, Pos, Prop };

  ObjectPropertyCommand() = default;

  Kind m_kind = Kind::Name;
  std::shared_ptr<ObjMapObject> m_obj;
  std::string m_label{"Edit object"};

  std::string m_old_name;
  std::string m_new_name;

  Pointf m_old_pos;
  Pointf m_new_pos;

  std::string m_key;
  std::optional<PropertyValue> m_old_prop;
  PropertyValue m_new_prop;
};

} // namespace flexlay

#endif
