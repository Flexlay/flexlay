// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map.hpp"
#include "flexlay/editor_map_widget.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/workspace.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>

#include <memory>
#include <random>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName("flexlay-simpleed");
  QApplication::setOrganizationName("Flexlay");

  constexpr int tile_size = 32;
  auto tileset = std::make_shared<flexlay::Tileset>(tile_size);
  tileset->add_tile(1, flexlay::Tile(flexlay::Color(70, 130, 180)));
  tileset->add_tile(2, flexlay::Tile(flexlay::Color(60, 179, 113)));
  tileset->add_tile(3, flexlay::Tile(flexlay::Color(205, 92, 92)));
  tileset->add_tile(4, flexlay::Tile(flexlay::Color(238, 221, 130)));
  tileset->add_tile(5, flexlay::Tile(flexlay::Color(147, 112, 219)));

  constexpr int map_w = 40;
  constexpr int map_h = 24;
  auto tilemap = std::make_shared<flexlay::TilemapLayer>(tileset, map_w, map_h);
  tilemap->set_name("ground");
  tilemap->set_draw_grid(true);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 5);
  for (int y = 0; y < map_h; ++y) {
    for (int x = 0; x < map_w; ++x) {
      int id = dist(rng);
      if ((x + y) % 7 == 0) {
        id = 0;
      }
      tilemap->field().put(x, y, static_cast<uint32_t>(id));
    }
  }

  auto editor_map = std::make_shared<flexlay::EditorMap>();
  editor_map->set_background_color(flexlay::Color(45, 45, 55));
  editor_map->set_bounding_rect(flexlay::Rectf(
    0, 0, static_cast<float>(map_w * tile_size), static_cast<float>(map_h * tile_size)));
  editor_map->add_layer(tilemap);

  auto workspace = std::make_shared<flexlay::Workspace>();
  workspace->set_map(editor_map);
  workspace->state().set_size(960, 600);
  workspace->state().set_pos(flexlay::Pointf(static_cast<float>(map_w * tile_size) / 2.0f,
                                             static_cast<float>(map_h * tile_size) / 2.0f));
  workspace->state().set_zoom(1.0f);

  QMainWindow window;
  window.setWindowTitle(QStringLiteral("Flexlay Simple Editor (C++)"));
  auto* map_widget = new flexlay::EditorMapWidget(&window);
  map_widget->set_workspace(workspace);
  window.setCentralWidget(map_widget);
  window.statusBar()->showMessage(
    QStringLiteral("Middle-drag: pan  |  Wheel: zoom  |  %1x%2 tiles @ %3px")
      .arg(map_w).arg(map_h).arg(tile_size));
  window.resize(960, 640);
  window.show();

  return app.exec();
}
