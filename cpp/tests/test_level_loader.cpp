// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/object_layer.hpp"
#include "flexlay/supertux/level_loader.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/util/sexpr.hpp"

#include <iostream>
#include <memory>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  auto level = supertux::load_level_text(R"(
    (supertux-level
      (version 4)
      (name "Test Level")
      (sectors
        (sector
          (name "main")
          (objects
            (tilemap
              (solid #t)
              (z-pos 0)
              (width 3)
              (height 2)
              (tiles 1 2 0 3 0 1)
            )
            (spawnpoint
              (name "main")
              (x 96)
              (y 64)
            )
            (bonusblock
              (x 128)
              (y 160)
              (data 1)
            )
            (camera (mode "normal"))
            (ambient-light 1 1 1)
          )
        )
      )
    )
  )");
  CHECK(level.version == 4);
  CHECK(level.name == "Test Level");
  CHECK(level.tilemaps().size() == 1);
  CHECK(level.tilemaps()[0].tiles[0] == 1);
  CHECK(level.objects().size() == 2);
  CHECK(level.objects()[0].type == "spawnpoint");
  CHECK(level.objects()[0].x == 96);
  CHECK(level.objects()[0].props.get_as<std::string>("name").value_or("") == "main");
  CHECK(level.objects()[1].type == "bonusblock");
  CHECK(level.objects()[1].props.get_as<int>("data").value_or(0) == 1);

  auto tileset = std::make_shared<Tileset>(32);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  tileset->add_tile(2, Tile(Color(0, 255, 0)));
  tileset->add_tile(3, Tile(Color(0, 0, 255)));
  auto map = supertux::editor_map_from_level(level, tileset);
  // tilemap + objects layer
  CHECK(map->layers().size() == 2);
  bool found_objects = false;
  for (const auto& layer : map->layers()) {
    if (auto ol = std::dynamic_pointer_cast<ObjectLayer>(layer)) {
      found_objects = true;
      CHECK(ol->objects().size() == 2);
    }
  }
  CHECK(found_objects);

  // Modern SuperTux: sector as root child, no (objects) wrapper
  auto modern = supertux::load_level_text(R"(
    (supertux-level
      (version 3)
      (name "Modern")
      (tileset "/images/tiles.strf")
      (sector
        (name "main")
        (tilemap
          (solid #t)
          (width 2)
          (height 1)
          (tiles 1 2)
        )
        (spawnpoint (name "main") (x 10) (y 20))
      )
    )
  )");
  CHECK(modern.sectors.size() == 1);
  CHECK(modern.tilemaps().size() == 1);
  CHECK(modern.objects().size() == 1);
  CHECK(modern.objects()[0].type == "spawnpoint");

  // Worldmap-style objects (tile coords); is_worldmap scales in editor_map_from_level
  auto wm = supertux::load_level_text(R"(
    (supertux-level
      (version 3)
      (name "WM")
      (sector
        (name "main")
        (tilemap (width 4) (height 4) (tiles 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0))
        (level (level "a.stl") (x 3) (y 5))
        (worldmap-spawnpoint (name "main") (x 1) (y 2))
      )
    )
  )");
  wm.is_worldmap = true;
  CHECK(wm.objects().size() == 2);
  auto wmap = supertux::editor_map_from_level(wm, tileset);
  found_objects = false;
  for (const auto& layer : wmap->layers()) {
    if (auto ol = std::dynamic_pointer_cast<ObjectLayer>(layer)) {
      found_objects = true;
      CHECK(ol->objects().size() == 2);
      // tile coords * 32
      bool saw_scaled = false;
      for (const auto& o : ol->objects()) {
        if (o->get_pos().x == 96.0f && o->get_pos().y == 160.0f) {
          saw_scaled = true;
        }
      }
      CHECK(saw_scaled);
    }
  }
  CHECK(found_objects);

  std::cout << "test_level_loader: ok\n";
  return 0;
}
