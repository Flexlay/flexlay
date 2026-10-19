// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COMMANDS_COMMAND_HPP
#define FLEXLAY_COMMANDS_COMMAND_HPP

#include <memory>
#include <string>

namespace flexlay {

class Command
{
public:
  virtual ~Command() = default;
  virtual void execute() = 0;
  virtual void undo() = 0;
  virtual void redo() { execute(); }
  virtual std::string name() const { return "Command"; }
};

using CommandPtr = std::shared_ptr<Command>;

} // namespace flexlay

#endif
