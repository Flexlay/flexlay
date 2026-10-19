// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_TOOL_HPP
#define FLEXLAY_TOOLS_TOOL_HPP

#include "flexlay/graphic_context.hpp"
#include "flexlay/math/point.hpp"

namespace flexlay {

class Workspace;

class Tool
{
public:
  virtual ~Tool() = default;

  virtual void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) {}
  virtual void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) {}
  virtual void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) {}
  virtual void draw(GraphicContext& /*gc*/, Workspace& /*workspace*/) {}
};

} // namespace flexlay

#endif
