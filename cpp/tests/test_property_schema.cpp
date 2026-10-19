// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/property_schema.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;
  using namespace flexlay::supertux;

  PropertyBag bag;
  ensure_property_schema(bag, "spawnpoint");
  CHECK(bag.has("name"));
  auto name = bag.get_as<std::string>("name");
  CHECK(name && *name == "main");

  bag.set("name", std::string("custom"));
  ensure_property_schema(bag, "spawnpoint");
  name = bag.get_as<std::string>("name");
  CHECK(name && *name == "custom"); // not overwritten

  ensure_property_schema(bag, "bonusblock");
  CHECK(bag.has("data"));
  CHECK(bag.has("script"));

  bag.set("kind", std::string("x"));
  bag.set("extra", 42);
  auto keys = property_display_keys(bag, "bonusblock");
  bool saw_extra = false;
  bool saw_kind = false;
  for (const auto& k : keys) {
    if (k == "extra") saw_extra = true;
    if (k == "kind") saw_kind = true;
  }
  CHECK(saw_extra);
  CHECK(!saw_kind);
  CHECK(property_label("bonusblock", "data") == "Contents");

  std::cout << "test_property_schema: ok\n";
  return 0;
}
