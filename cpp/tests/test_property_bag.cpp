// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/property_bag.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  PropertyBag bag;
  bag.set("hp", 3);
  bag.set("name", std::string("tux"));
  bag.set("flying", false);
  bag.set("speed", 1.5f);
  bag.set("tint", Color(255, 0, 0));
  bag.set("spawn", Pointf(10.0f, 20.0f));

  CHECK(bag.size() == 6);
  CHECK(bag.has("hp"));
  CHECK(bag.get_as<int>("hp").value_or(0) == 3);
  CHECK(bag.get_as<std::string>("name").value_or("") == "tux");
  CHECK(bag.get_as<bool>("flying").value_or(true) == false);
  CHECK(bag.get_as<float>("speed").value_or(0.0f) == 1.5f);
  CHECK(bag.get_as<Color>("tint").value_or(Color()).r == 255);
  CHECK(bag.get_as<Pointf>("spawn").value_or(Pointf()).x == 10.0f);
  CHECK(!bag.get_as<int>("name").has_value()); // wrong type

  ObjMapRectObject obj(Pointf(0, 0), Sizef(16, 16), Color(1, 2, 3));
  obj.properties().set("id", 42);
  CHECK(obj.properties().get_as<int>("id").value_or(0) == 42);

  std::cout << "test_property_bag: ok\n";
  return 0;
}
