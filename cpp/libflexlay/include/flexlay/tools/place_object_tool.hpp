// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_PLACE_OBJECT_TOOL_HPP
#define FLEXLAY_TOOLS_PLACE_OBJECT_TOOL_HPP

#include "flexlay/object_layer.hpp"
#include "flexlay/object_prototype.hpp"
#include "flexlay/tools/tool.hpp"
#include "flexlay/workspace.hpp"

#include <functional>
#include <memory>
#include <optional>

namespace flexlay {

/** LMB places the current ObjectPrototype on the object layer; RMB cancels tool. */
class PlaceObjectTool : public Tool
{
public:
  PlaceObjectTool() = default;

  void set_object_layer(std::shared_ptr<ObjectLayer> layer) { m_layer = std::move(layer); }
  void set_prototype(ObjectPrototype proto) { m_proto = std::move(proto); }
  const std::optional<ObjectPrototype>& prototype() const { return m_proto; }
  void clear_prototype() { m_proto.reset(); }
  void set_datadir(std::string datadir) { m_datadir = std::move(datadir); }

  /** Called after a successful place (with the new object). */
  std::function<void(std::shared_ptr<ObjMapObject>)> on_placed;
  /** Called when RMB cancels placement mode. */
  std::function<void()> on_cancelled;

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;
  void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) override;
  void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) override;
  void draw(GraphicContext& gc, Workspace& workspace) override;

private:
  std::shared_ptr<ObjectLayer> m_layer;
  std::optional<ObjectPrototype> m_proto;
  std::string m_datadir;
  Pointf m_cursor;
  bool m_have_cursor = false;
};

} // namespace flexlay

#endif
