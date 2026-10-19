// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_TOOLS_PLACE_OBJECT_TOOL_HPP
#define FLEXLAY_TOOLS_PLACE_OBJECT_TOOL_HPP

#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/object_prototype.hpp"
#include "flexlay/sprite.hpp"
#include "flexlay/tools/tool.hpp"
#include "flexlay/workspace.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace flexlay {

class PlaceObjectTool : public Tool
{
public:
  void set_object_layer(std::shared_ptr<ObjectLayer> layer) { m_layer = std::move(layer); }
  void set_prototype(ObjectPrototype proto);
  void clear_prototype();
  void set_datadir(std::string datadir);
  void set_snap(bool on) { m_snap = on; }
  bool snap() const { return m_snap; }
  void set_snap_size(float s) { m_snap_size = s; }

  std::function<void(std::shared_ptr<ObjMapObject>)> on_placed;
  std::function<void()> on_cancelled;

  void on_mouse_down(Workspace& workspace, const Pointf& world_pos, int button) override;
  void on_mouse_move(Workspace& workspace, const Pointf& world_pos, int buttons) override;
  void on_mouse_up(Workspace& workspace, const Pointf& world_pos, int button) override;
  void draw(GraphicContext& gc, Workspace& workspace) override;

private:
  void reload_preview();
  Pointf snap_pos(const Pointf& p) const;

  std::shared_ptr<ObjectLayer> m_layer;
  std::optional<ObjectPrototype> m_proto;
  std::string m_datadir;
  Pointf m_cursor;
  bool m_have_cursor = false;
  bool m_snap = true;
  float m_snap_size = 32.0f;

  std::optional<Sprite> m_preview_sprite;
  std::optional<Rectf> m_preview_hitbox; // sprite-local
};

} // namespace flexlay

#endif
