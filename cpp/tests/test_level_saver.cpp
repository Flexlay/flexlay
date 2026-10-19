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
      (author "Test Author")
      (license "CC-BY-SA 4.0")
      (contact "test@example.com")
      (tileset "images/tiles.stts")
      (target-time 120)
      (sectors
        (sector
          (name "main")
          (objects
            (gravity 12.5)
            (ambient-light 0.5 0.6 0.7 1)
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
  CHECK(level.tilemaps().size() == 1);
  CHECK(level.objects().size() == 2);

  const std::string text = supertux::save_level_text(level);
  auto again = supertux::load_level_text(text);
  CHECK(again.name == "Roundtrip");
  CHECK(again.author == "Test Author");
  CHECK(again.license == "CC-BY-SA 4.0");
  CHECK(again.contact == "test@example.com");
  CHECK(again.tileset_path == "images/tiles.stts");
  CHECK(again.target_time == 120);
  CHECK(again.has_gravity());
  CHECK(again.gravity() == 12.5f);
  CHECK(again.has_ambient_light());
  CHECK(again.ambient_r() == 0.5f);
  CHECK(again.ambient_g() == 0.6f);
  CHECK(again.ambient_b() == 0.7f);
  CHECK(again.tilemaps().size() == 1);
  CHECK(again.tilemaps()[0].width == 2);
  CHECK(again.tilemaps()[0].solid == true);
  CHECK(again.tilemaps()[0].tiles[0] == 1);
  CHECK(again.tilemaps()[0].tiles[3] == 0);
  CHECK(again.objects().size() == 2);
  CHECK(again.objects()[0].type == "spawnpoint");
  CHECK(again.objects()[0].x == 10);
  CHECK(again.objects()[0].props.get_as<std::string>("name").value_or("") == "main");
  CHECK(again.objects()[1].type == "bonusblock");
  CHECK(again.objects()[1].props.get_as<int>("data").value_or(0) == 2);

  auto tileset = std::make_shared<Tileset>(32);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  auto map = supertux::editor_map_from_level(level, tileset);
  auto from_map = supertux::level_from_editor_map(*map, "FromMap", "main", 4, &level);
  CHECK(from_map.author == "Test Author");
  CHECK(from_map.tileset_path == "images/tiles.stts");
  CHECK(from_map.has_gravity());
  CHECK(from_map.gravity() == 12.5f);
  CHECK(from_map.has_ambient_light());
  CHECK(from_map.tilemaps().size() == 1);
  CHECK(from_map.objects().size() == 2);
  const std::string text2 = supertux::save_level_text(from_map);
  auto third = supertux::load_level_text(text2);
  CHECK(third.tilemaps()[0].tiles[1] == 2);
  CHECK(third.objects().size() == 2);

  // Multi-sector load + save
  auto multi = supertux::load_level_text(R"(
    (supertux-level
      (version 4)
      (name "Multi")
      (sectors
        (sector
          (name "main")
          (objects
            (gravity 10)
            (tilemap (solid #t) (width 1) (height 1) (tiles 1))
          )
        )
        (sector
          (name "secret")
          (objects
            (gravity 5)
            (tilemap (solid #f) (width 1) (height 1) (tiles 2))
            (spawnpoint (name "s") (x 1) (y 2))
          )
        )
      )
    )
  )");
  CHECK(multi.sectors.size() == 2);
  CHECK(multi.sector_names().size() == 2);
  CHECK(multi.sector_names()[1] == "secret");
  CHECK(multi.select_sector("secret"));
  CHECK(multi.sector_name() == "secret");
  CHECK(multi.gravity() == 5.0f);
  CHECK(multi.objects().size() == 1);
  const std::string multi_text = supertux::save_level_text(multi);
  auto multi2 = supertux::load_level_text(multi_text);
  CHECK(multi2.sectors.size() == 2);
  CHECK(multi2.select_sector("secret"));
  CHECK(multi2.objects().size() == 1);

  std::cout << "test_level_saver: ok\n";
  return 0;
}
