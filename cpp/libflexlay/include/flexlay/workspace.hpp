// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_WORKSPACE_HPP
#define FLEXLAY_WORKSPACE_HPP

#include "flexlay/editor_map.hpp"
#include "flexlay/graphic_context.hpp"
#include "flexlay/graphic_context_state.hpp"

#include <memory>

namespace flexlay {

class Workspace
{
public:
  Workspace() = default;

  void set_map(std::shared_ptr<EditorMap> map) { m_map = std::move(map); }
  std::shared_ptr<EditorMap> get_map() const { return m_map; }
  EditorMap* map() { return m_map.get(); }
  const EditorMap* map() const { return m_map.get(); }

  GraphicContextState& state() { return m_state; }
  const GraphicContextState& state() const { return m_state; }

  void draw(GraphicContext& gc) const
  {
    if (!m_map) {
      return;
    }
    m_state.push(gc);
    gc.set_clip_rect(m_state.get_clip_rect());
    m_map->draw(gc);
    m_state.pop(gc);
  }

private:
  std::shared_ptr<EditorMap> m_map;
  GraphicContextState m_state;
};

} // namespace flexlay

#endif
