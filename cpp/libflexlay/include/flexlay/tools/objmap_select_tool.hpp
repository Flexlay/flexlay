// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_OBJMAP_SELECT_TOOL_HPP
#define FLEXLAY_TOOLS_OBJMAP_SELECT_TOOL_HPP

#include "flexlay/commands/object_move_command.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/tools/tool.hpp"
#include "flexlay/workspace.hpp"

#include <memory>

namespace flexlay {

class ObjMapSelectTool : public Tool
{
public:
  ObjMapSelectTool() = default;

  void set_object_layer(std::shared_ptr<ObjectLayer> layer) { m_layer = std::move(layer); }

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;
  void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) override;
  void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) override;

  bool delete_selection(Workspace& workspace);

private:
  enum class Mode { idle, moving, resizing };

  std::shared_ptr<ObjectLayer> m_layer;
  Mode m_mode = Mode::idle;
  Pointf m_drag_start;
  std::shared_ptr<ObjectMoveCommand> m_move_cmd;

  std::shared_ptr<ObjMapRectObject> m_resize_obj;
  ResizeHandle m_resize_handle = ResizeHandle::none;
  Pointf m_resize_old_pos;
  Sizef m_resize_old_size;
};

} // namespace flexlay

#endif
