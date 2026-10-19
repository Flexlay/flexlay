// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/pixel_buffer.hpp"
#include "flexlay/supertux/tileset_loader.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  // --- subregion unit check (no Qt required for synthetic buffers) ---
  {
    auto sheet = PixelBuffer::from_color(Size(64, 32), Color(10, 20, 30, 255));
    sheet.set_pixel(0, 0, Color(1, 2, 3, 255));
    sheet.set_pixel(32, 0, Color(4, 5, 6, 255));
    sheet.set_pixel(0, 16, Color(7, 8, 9, 255));
    auto a = sheet.subregion(0, 0, 32, 32);
    CHECK(a.width() == 32);
    CHECK(a.height() == 32);
    CHECK(a.pixel(0, 0) == Color(1, 2, 3, 255));
    // out of source bounds → transparent
    CHECK(a.pixel(0, 16) == Color(7, 8, 9, 255));
    auto b = sheet.subregion(32, 0, 32, 32);
    CHECK(b.pixel(0, 0) == Color(4, 5, 6, 255));
  }

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
        (tile
          (id 5)
          (images (region "tiles/doodads/statue.png" 0 0 32 96))
        )
        (tile
          (id 6)
          (editor-images "tiles/editor/preview.png")
          (images "tiles/game/runtime.png")
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
  CHECK(tileset->has_tile(5));
  CHECK(tileset->has_tile(6));
  CHECK(tileset->has_tile(10));
  CHECK(tileset->has_tile(11));
  CHECK(tileset->has_tile(12));
  CHECK(!tileset->has_tile(0));
  CHECK(tileset->create(1).get_filename().has_value());
  CHECK(*tileset->create(1).get_filename() == "tiles/snow/block.png");
  CHECK(tileset->create(5).get_filename().has_value());
  CHECK(*tileset->create(5).get_filename() == "tiles/doodads/statue.png");
  // editor-images preferred over images
  CHECK(tileset->create(6).get_filename().has_value());
  CHECK(*tileset->create(6).get_filename() == "tiles/editor/preview.png");
  // bulk still records sheet path on each id
  CHECK(tileset->create(10).get_filename().has_value());
  CHECK(*tileset->create(10).get_filename() == "tiles/snow/sheet.png");
  CHECK(meta.tilegroups.size() == 1);
  CHECK(meta.tilegroups[0].name == "Snow");
  CHECK(meta.tilegroups[0].tiles.size() == 3);
  CHECK(meta.tile_count >= 6);

  std::cout << "test_tileset_loader: ok\n";
  return 0;
}
