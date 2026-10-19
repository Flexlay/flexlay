// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_COMMAND_GROUP_HPP
#define FLEXLAY_COMMANDS_COMMAND_GROUP_HPP

#include "flexlay/commands/command.hpp"

#include <vector>

namespace flexlay {

/** Composite undoable: execute forward, undo reverse. */
class CommandGroup : public Command
{
public:
  CommandGroup() = default;
  explicit CommandGroup(std::string name) : m_name(std::move(name)) {}

  void add(CommandPtr cmd)
  {
    if (cmd) {
      m_commands.push_back(std::move(cmd));
    }
  }

  void execute() override
  {
    for (auto& c : m_commands) {
      c->execute();
    }
  }

  void undo() override
  {
    for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
      (*it)->undo();
    }
  }

  std::string name() const override { return m_name; }
  bool empty() const { return m_commands.empty(); }
  size_t size() const { return m_commands.size(); }

private:
  std::string m_name{"CommandGroup"};
  std::vector<CommandPtr> m_commands;
};

} // namespace flexlay

#endif
