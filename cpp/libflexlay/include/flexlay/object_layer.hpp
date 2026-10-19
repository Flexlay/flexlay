// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJECT_LAYER_HPP
#define FLEXLAY_OBJECT_LAYER_HPP

#include "flexlay/layer.hpp"
#include "flexlay/objmap_object.hpp"

#include <memory>
#include <vector>

namespace flexlay {

class ObjectLayer : public Layer
{
public:
  ObjectLayer() { m_name = "objects"; }

  void draw(GraphicContext& gc, const Point& pos = Point(0, 0)) override;

  void add_object(std::shared_ptr<ObjMapObject> obj);
  void remove_object(const std::shared_ptr<ObjMapObject>& obj);

  std::shared_ptr<ObjMapObject> find_object(const Pointf& click_pos) const;

  const std::vector<std::shared_ptr<ObjMapObject>>& objects() const { return m_objects; }
  std::vector<std::shared_ptr<ObjMapObject>>& objects() { return m_objects; }

  void clear_selection();
  std::vector<std::shared_ptr<ObjMapObject>> selected_objects() const;

  /** Z-order: later entries are drawn on top. */
  void raise_object(const std::shared_ptr<ObjMapObject>& obj);
  void lower_object(const std::shared_ptr<ObjMapObject>& obj);
  void raise_object_to_top(const std::shared_ptr<ObjMapObject>& obj);
  void lower_object_to_bottom(const std::shared_ptr<ObjMapObject>& obj);

private:
  std::vector<std::shared_ptr<ObjMapObject>> m_objects;
};

} // namespace flexlay

#endif
