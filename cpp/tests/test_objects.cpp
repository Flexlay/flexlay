// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_add_command.hpp"
#include "flexlay/commands/object_transform_command.hpp"
#include "flexlay/commands/object_delete_command.hpp"
#include "flexlay/commands/object_move_command.hpp"
#include "flexlay/editor_map.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"

#include <iostream>
#include <memory>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  auto layer = std::make_shared<ObjectLayer>();
  auto rect = std::make_shared<ObjMapRectObject>(
    Pointf(10, 20), Sizef(40, 30), Color(255, 0, 0, 128));
  layer->add_object(rect);
  CHECK(layer->objects().size() == 1);
  CHECK(rect->get_bound_rect() == Rectf(10, 20, 50, 50));
  CHECK(rect->is_inside(Pointf(15, 25)));
  CHECK(!rect->is_inside(Pointf(0, 0)));

  auto hit = layer->find_object(Pointf(15, 25));
  CHECK(hit == rect);

  auto cmd = std::make_shared<ObjectMoveCommand>();
  cmd->add_obj(rect);
  cmd->move_by(Pointf(5, 7));
  CHECK(rect->get_pos() == Pointf(15, 27));

  auto map = std::make_shared<EditorMap>();
  map->add_layer(layer);
  map->execute(cmd);
  map->undo();
  CHECK(rect->get_pos() == Pointf(10, 20));
  map->redo();
  CHECK(rect->get_pos() == Pointf(15, 27));

  auto del = std::make_shared<ObjectDeleteCommand>(layer);
  del->add_object(rect);
  map->execute(del);
  CHECK(layer->objects().empty());
  map->undo();
  CHECK(layer->objects().size() == 1);

  auto extra = std::make_shared<ObjMapRectObject>(Pointf(0, 0), Sizef(10, 10), Color(0, 255, 0));
  auto add = std::make_shared<ObjectAddCommand>(layer);
  add->add_object(extra);
  map->execute(add);
  CHECK(layer->objects().size() == 2);
  map->undo();
  CHECK(layer->objects().size() == 1);

  // Resize handle + transform undo (pos is 15,27 size 40x30 after earlier redo)
  rect->set_selected(true);
  const Pointf tl = rect->get_pos();
  CHECK(rect->hit_handle(tl) == ResizeHandle::top_left);
  const Pointf br(tl.x + 70, tl.y + 70);
  rect->resize_to_handle(ResizeHandle::bottom_right, br);
  CHECK(rect->get_size().width == 70);
  CHECK(rect->get_size().height == 70);

  const Pointf saved_pos = rect->get_pos();
  const Sizef saved_size = rect->get_size();
  auto xform = std::make_shared<ObjectTransformCommand>(
    rect, Pointf(15, 27), Sizef(40, 30), saved_pos, saved_size);
  map->execute(xform);
  map->undo();
  CHECK(rect->get_size() == Sizef(40, 30));
  CHECK(rect->get_pos() == Pointf(15, 27));

  std::cout << "test_objects: ok\n";
  return 0;
}
