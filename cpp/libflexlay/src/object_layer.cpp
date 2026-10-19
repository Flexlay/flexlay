// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/object_layer.hpp"

#include <algorithm>

namespace flexlay {

void ObjectLayer::draw(GraphicContext& gc, const Point& /*pos*/)
{
  if (m_hidden) {
    return;
  }
  const Rectf clip = gc.get_clip_rect();
  for (const auto& obj : m_objects) {
    if (!obj || !obj->to_draw()) {
      continue;
    }
    const Rectf bound = obj->get_bound_rect();
    if (clip.is_overlapped(bound)) {
      obj->draw(gc);
    }
  }
}

void ObjectLayer::add_object(std::shared_ptr<ObjMapObject> obj)
{
  if (obj) {
    m_objects.push_back(std::move(obj));
  }
}

void ObjectLayer::remove_object(const std::shared_ptr<ObjMapObject>& obj)
{
  m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), obj), m_objects.end());
}

std::shared_ptr<ObjMapObject> ObjectLayer::find_object(const Pointf& click_pos) const
{
  for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
    if (*it && (*it)->is_inside(click_pos)) {
      return *it;
    }
  }
  return nullptr;
}

void ObjectLayer::clear_selection()
{
  for (auto& obj : m_objects) {
    if (obj) {
      obj->set_selected(false);
    }
  }
}

std::vector<std::shared_ptr<ObjMapObject>> ObjectLayer::selected_objects() const
{
  std::vector<std::shared_ptr<ObjMapObject>> result;
  for (const auto& obj : m_objects) {
    if (obj && obj->is_selected()) {
      result.push_back(obj);
    }
  }
  return result;
}



void ObjectLayer::raise_object(const std::shared_ptr<ObjMapObject>& obj)
{
  auto it = std::find(m_objects.begin(), m_objects.end(), obj);
  if (it == m_objects.end()) {
    return;
  }
  auto next = std::next(it);
  if (next == m_objects.end()) {
    return;
  }
  std::iter_swap(it, next);
}

void ObjectLayer::lower_object(const std::shared_ptr<ObjMapObject>& obj)
{
  auto it = std::find(m_objects.begin(), m_objects.end(), obj);
  if (it == m_objects.begin() || it == m_objects.end()) {
    return;
  }
  std::iter_swap(it, std::prev(it));
}

void ObjectLayer::raise_object_to_top(const std::shared_ptr<ObjMapObject>& obj)
{
  auto it = std::find(m_objects.begin(), m_objects.end(), obj);
  if (it == m_objects.end()) {
    return;
  }
  auto ptr = *it;
  m_objects.erase(it);
  m_objects.push_back(std::move(ptr));
}

void ObjectLayer::lower_object_to_bottom(const std::shared_ptr<ObjMapObject>& obj)
{
  auto it = std::find(m_objects.begin(), m_objects.end(), obj);
  if (it == m_objects.end()) {
    return;
  }
  auto ptr = *it;
  m_objects.erase(it);
  m_objects.insert(m_objects.begin(), std::move(ptr));
}

} // namespace flexlay
