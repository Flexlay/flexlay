// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/paint_command.hpp"
#include "flexlay/tile_brush.hpp"
#include "flexlay/editor_map.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"

#include <iostream>
#include <memory>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  auto tileset = std::make_shared<Tileset>(16);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  auto layer = std::make_shared<TilemapLayer>(tileset, 4, 4);
  CHECK(layer->field().at(1, 1) == 0);

  auto cmd = std::make_shared<PaintCommand>(layer);
  cmd->paint_cell(1, 1, 1);
  cmd->paint_cell(2, 1, 1);
  CHECK(layer->field().at(1, 1) == 1);
  CHECK(layer->field().at(2, 1) == 1);

  auto map = std::make_shared<EditorMap>();
  map->add_layer(layer);
  map->execute(cmd);
  CHECK(map->undo_stack_size() == 1);

  map->undo();
  CHECK(layer->field().at(1, 1) == 0);
  CHECK(layer->field().at(2, 1) == 0);
  CHECK(map->redo_stack_size() == 1);

  map->redo();
  CHECK(layer->field().at(1, 1) == 1);

  auto cmd2 = std::make_shared<PaintCommand>(layer);
  cmd2->paint_cell(1, 1, 0);
  map->execute(cmd2);
  CHECK(layer->field().at(1, 1) == 0);
  map->undo();
  CHECK(layer->field().at(1, 1) == 1);

  // Multi-tile brush stamp
  auto cmd3 = std::make_shared<PaintCommand>(layer);
  auto brush = TileBrush::filled(2, 2, 1, true);
  cmd3->paint_brush(0, 0, brush);
  CHECK(layer->field().at(0, 0) == 1);
  CHECK(layer->field().at(1, 0) == 1);
  CHECK(layer->field().at(0, 1) == 1);
  CHECK(layer->field().at(1, 1) == 1);
  map->execute(cmd3);
  map->undo();
  CHECK(layer->field().at(0, 0) == 0);

  // Transparent zero skips
  auto sparse = TileBrush(2, 1);
  sparse.put(0, 0, 1);
  sparse.put(1, 0, 0);
  sparse.set_opaque(false);
  layer->field().put(0, 2, 0);
  layer->field().put(1, 2, 2);
  auto cmd4 = std::make_shared<PaintCommand>(layer);
  cmd4->paint_brush(0, 2, sparse);
  CHECK(layer->field().at(0, 2) == 1);
  CHECK(layer->field().at(1, 2) == 2); // unchanged

  // Copy brush from field region
  layer->field().put(2, 2, 1);
  layer->field().put(3, 2, 1);
  layer->field().put(2, 3, 0);
  layer->field().put(3, 3, 1);
  auto copied = TileBrush::from_field(layer->field(), 2, 2, 2, 2);
  CHECK(copied.width() == 2);
  CHECK(copied.height() == 2);
  CHECK(copied.at(0, 0) == 1);
  CHECK(copied.at(1, 0) == 1);
  CHECK(copied.at(0, 1) == 0);
  CHECK(copied.at(1, 1) == 1);
  CHECK(!copied.is_opaque());

  std::cout << "test_paint: ok\n";
  return 0;
}
