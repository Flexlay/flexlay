// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/level_loader.hpp"
#include "flexlay/supertux/level_saver.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tileset.hpp"

#include <iostream>
#include <memory>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  const char* src = R"(
    (supertux-level
      (version 4)
      (name "Roundtrip")
      (sectors
        (sector
          (name "main")
          (objects
            (tilemap
              (name "ground")
              (solid #t)
              (z-pos 0)
              (width 2)
              (height 2)
              (tiles 1 2 3 0)
            )
            (spawnpoint
              (name "main")
              (x 10)
              (y 20)
            )
            (bonusblock
              (x 32)
              (y 64)
              (data 2)
            )
          )
        )
      )
    )
  )";

  auto level = supertux::load_level_text(src);
  CHECK(level.tilemaps.size() == 1);
  CHECK(level.objects.size() == 2);

  const std::string text = supertux::save_level_text(level);
  auto again = supertux::load_level_text(text);
  CHECK(again.name == "Roundtrip");
  CHECK(again.tilemaps.size() == 1);
  CHECK(again.tilemaps[0].width == 2);
  CHECK(again.tilemaps[0].solid == true);
  CHECK(again.tilemaps[0].tiles[0] == 1);
  CHECK(again.tilemaps[0].tiles[3] == 0);
  CHECK(again.objects.size() == 2);
  CHECK(again.objects[0].type == "spawnpoint");
  CHECK(again.objects[0].x == 10);
  CHECK(again.objects[0].props.get_as<std::string>("name").value_or("") == "main");
  CHECK(again.objects[1].type == "bonusblock");
  CHECK(again.objects[1].props.get_as<int>("data").value_or(0) == 2);

  auto tileset = std::make_shared<Tileset>(32);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  auto map = supertux::editor_map_from_level(level, tileset);
  auto from_map = supertux::level_from_editor_map(*map, "FromMap");
  CHECK(from_map.tilemaps.size() == 1);
  CHECK(from_map.objects.size() == 2);
  const std::string text2 = supertux::save_level_text(from_map);
  auto third = supertux::load_level_text(text2);
  CHECK(third.tilemaps[0].tiles[1] == 2);
  CHECK(third.objects.size() == 2);

  std::cout << "test_level_saver: ok\n";
  return 0;
}
