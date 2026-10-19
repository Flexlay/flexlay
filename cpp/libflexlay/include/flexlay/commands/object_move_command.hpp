// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_OBJECT_MOVE_COMMAND_HPP
#define FLEXLAY_COMMANDS_OBJECT_MOVE_COMMAND_HPP

#include "flexlay/commands/command.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/objmap_object.hpp"

#include <memory>
#include <vector>

namespace flexlay {

class ObjectMoveCommand : public Command
{
public:
  ObjectMoveCommand() = default;

  void add_obj(std::shared_ptr<ObjMapObject> obj);
  void move_by(const Pointf& delta);

  void execute() override {}
  void undo() override;
  void redo() override;
  std::string name() const override { return "Move objects"; }

  bool empty() const { return m_entries.empty(); }

private:
  struct Entry
  {
    std::shared_ptr<ObjMapObject> obj;
    Pointf old_pos;
    Pointf new_pos;
  };
  std::vector<Entry> m_entries;
};

} // namespace flexlay

#endif
