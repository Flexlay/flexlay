// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/object_property_command.hpp"
#include "flexlay/objmap_rect_object.hpp"

#include <iostream>
#include <memory>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  auto obj = std::make_shared<ObjMapRectObject>(Pointf(1, 2), Sizef(16, 16), Color(1, 2, 3));
  obj->set_name("a");
  obj->properties().set("hp", 3);

  {
    auto cmd = ObjectPropertyCommand::set_name(obj, "a", "b");
    CHECK(cmd.changed());
    cmd.execute();
    CHECK(obj->name() == "b");
    cmd.undo();
    CHECK(obj->name() == "a");
  }
  {
    auto cmd = ObjectPropertyCommand::set_pos(obj, Pointf(1, 2), Pointf(10, 20));
    cmd.execute();
    CHECK(obj->get_pos().x == 10);
    cmd.undo();
    CHECK(obj->get_pos().x == 1);
  }
  {
    auto old = obj->properties().get("hp");
    auto cmd = ObjectPropertyCommand::set_property(obj, "hp", old, 9);
    cmd.execute();
    CHECK(obj->properties().get_as<int>("hp").value_or(0) == 9);
    cmd.undo();
    CHECK(obj->properties().get_as<int>("hp").value_or(0) == 3);
  }

  std::cout << "test_object_property_command: ok\n";
  return 0;
}
