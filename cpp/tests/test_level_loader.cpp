// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

  const SExpr root = sexpr_parse(R"(
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
            (snowball (x 10) (y 20))
          )
        )
      )
    )
  )");
  CHECK(root.tag() == "supertux-level");

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
          )
        )
      )
    )
  )");
  CHECK(level.version == 4);
  CHECK(level.name == "Test Level");
  CHECK(level.tilemaps.size() == 1);
  CHECK(level.tilemaps[0].width == 3);
  CHECK(level.tilemaps[0].height == 2);
  CHECK(level.tilemaps[0].tiles.size() == 6);
  CHECK(level.tilemaps[0].tiles[0] == 1);
  CHECK(level.tilemaps[0].tiles[5] == 1);

  auto tileset = std::make_shared<Tileset>(32);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  tileset->add_tile(2, Tile(Color(0, 255, 0)));
  tileset->add_tile(3, Tile(Color(0, 0, 255)));
  auto map = supertux::editor_map_from_level(level, tileset);
  CHECK(map->layers().size() == 1);

  std::cout << "test_level_loader: ok\n";
  return 0;
}
