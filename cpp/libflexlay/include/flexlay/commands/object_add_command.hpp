// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_OBJECT_ADD_COMMAND_HPP
#define FLEXLAY_COMMANDS_OBJECT_ADD_COMMAND_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_object.hpp"

#include <memory>
#include <vector>

namespace flexlay {

class ObjectAddCommand : public Command
{
public:
  explicit ObjectAddCommand(std::shared_ptr<ObjectLayer> layer);

  void add_object(std::shared_ptr<ObjMapObject> obj);

  void execute() override;
  void undo() override;
  void redo() override { execute(); }
  std::string name() const override { return "Add objects"; }

  bool empty() const { return m_objects.empty(); }

private:
  std::shared_ptr<ObjectLayer> m_layer;
  std::vector<std::shared_ptr<ObjMapObject>> m_objects;
  bool m_applied = false;
};

} // namespace flexlay

#endif
