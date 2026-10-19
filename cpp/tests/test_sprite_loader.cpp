// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/sprite_loader.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay::supertux;

  auto def = load_sprite_def_text(R"(
    (supertux-sprite
      (version 2)
      (actions
        (action
          (name "right")
          (images "a.png" "b.png")
        )
        (action
          (name "left")
          (mirror-action "right")
        )
        (action
          (name "default")
          (images "default.png")
        )
      )
    )
  )", "/tmp/fake");

  CHECK(def.version == 2);
  CHECK(def.actions.size() == 3);
  CHECK(def.actions[0].images.size() == 2);
  CHECK(def.actions[1].mirror_action == "right");

  auto path = preferred_image_path(def);
  CHECK(path.has_value());
  CHECK(*path == "/tmp/fake/default.png"); // prefers "default"

  // Prefer left when no default
  auto def2 = load_sprite_def_text(R"(
    (supertux-sprite
      (actions
        (action (name "right") (images "r.png"))
        (action (name "left") (images "l.png"))
      )
    )
  )", "/base");
  auto path2 = preferred_image_path(def2);
  CHECK(path2.has_value());
  CHECK(*path2 == "/base/l.png");

  // mirror-action resolution
  auto def3 = load_sprite_def_text(R"(
    (supertux-sprite
      (actions
        (action (name "left") (mirror-action "right"))
        (action (name "right") (images "only.png"))
      )
    )
  )", "/m");
  auto path3 = preferred_image_path(def3);
  CHECK(path3.has_value());
  CHECK(*path3 == "/m/only.png");

  std::cout << "test_sprite_loader: ok\n";
  return 0;
}
