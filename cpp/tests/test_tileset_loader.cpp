// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/tileset_loader.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  const char* text = R"(
    (supertux-tileset
      (tilegroups
        (tilegroup
          (name "Snow")
          (tiles 1 2 3)
        )
      )
      (tiles
        (tile
          (id 1)
          (images "tiles/snow/block.png")
          (solid #t)
        )
        (tile
          (id 2)
          (images "tiles/snow/slope.png")
        )
      )
      (tiles
        (ids 10 11 0 12)
        (width 2)
        (image "tiles/snow/sheet.png")
      )
    )
  )";

  auto [tileset, meta] = supertux::load_tileset_text(text);
  CHECK(tileset->has_tile(1));
  CHECK(tileset->has_tile(2));
  CHECK(tileset->has_tile(10));
  CHECK(tileset->has_tile(11));
  CHECK(tileset->has_tile(12));
  CHECK(!tileset->has_tile(0));
  CHECK(tileset->create(1).get_filename().has_value());
  CHECK(*tileset->create(1).get_filename() == "tiles/snow/block.png");
  CHECK(meta.tilegroups.size() == 1);
  CHECK(meta.tilegroups[0].name == "Snow");
  CHECK(meta.tilegroups[0].tiles.size() == 3);
  CHECK(meta.tile_count >= 5);

  std::cout << "test_tileset_loader: ok\n";
  return 0;
}
