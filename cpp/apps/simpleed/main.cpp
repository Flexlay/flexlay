// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map.hpp"
#include "flexlay/editor_map_widget.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/tools/tile_paint_tool.hpp"
#include "flexlay/workspace.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

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
      if ((x + y) % 7 == 0) id = 0;
      tilemap->field().put(x, y, static_cast<uint32_t>(id));
    }
  }

  auto editor_map = std::make_shared<flexlay::EditorMap>();
  editor_map->set_background_color(flexlay::Color(45, 45, 55));
  editor_map->set_bounding_rect(flexlay::Rectf(
    0, 0, static_cast<float>(map_w * tile_size), static_cast<float>(map_h * tile_size)));
  editor_map->add_layer(tilemap);

  auto paint_tool = std::make_shared<flexlay::TilePaintTool>();
  paint_tool->set_tilemap(tilemap);
  paint_tool->set_tile_id(1);

  auto workspace = std::make_shared<flexlay::Workspace>();
  workspace->set_map(editor_map);
  workspace->set_tool(paint_tool);
  workspace->state().set_size(960, 600);
  workspace->state().set_pos(flexlay::Pointf(static_cast<float>(map_w * tile_size) / 2.0f,
                                             static_cast<float>(map_h * tile_size) / 2.0f));

  QMainWindow window;
  window.setWindowTitle(QStringLiteral("Flexlay Simple Editor (C++)"));

  auto* central = new QWidget(&window);
  auto* layout = new QHBoxLayout(central);
  auto* sidebar = new QWidget(central);
  auto* side_layout = new QVBoxLayout(sidebar);
  side_layout->addWidget(new QLabel(QStringLiteral("Tile:"), sidebar));

  auto make_tile_btn = [&](uint32_t id, const QString& label, const QColor& color) {
    auto* btn = new QPushButton(label, sidebar);
    btn->setStyleSheet(QStringLiteral("background-color: %1; min-height: 28px;").arg(color.name()));
    QObject::connect(btn, &QPushButton::clicked, [paint_tool, id]() { paint_tool->set_tile_id(id); });
    side_layout->addWidget(btn);
  };
  make_tile_btn(0, QStringLiteral("0 Erase"), QColor(40, 40, 40));
  make_tile_btn(1, QStringLiteral("1 Blue"), QColor(70, 130, 180));
  make_tile_btn(2, QStringLiteral("2 Green"), QColor(60, 179, 113));
  make_tile_btn(3, QStringLiteral("3 Red"), QColor(205, 92, 92));
  make_tile_btn(4, QStringLiteral("4 Yellow"), QColor(238, 221, 130));
  make_tile_btn(5, QStringLiteral("5 Purple"), QColor(147, 112, 219));
  side_layout->addStretch(1);

  auto* map_widget = new flexlay::EditorMapWidget(central);
  map_widget->set_workspace(workspace);
  layout->addWidget(sidebar, 0);
  layout->addWidget(map_widget, 1);
  window.setCentralWidget(central);

  auto update_status = [&]() {
    window.statusBar()->showMessage(
      QStringLiteral("LMB paint | MMB pan | Wheel zoom | Ctrl+Z/Y undo/redo | tile=%1 | undo=%2 redo=%3")
        .arg(paint_tool->tile_id())
        .arg(editor_map->undo_stack_size())
        .arg(editor_map->redo_stack_size()));
  };
  QObject::connect(map_widget, &flexlay::EditorMapWidget::map_changed, update_status);
  update_status();

  window.resize(1100, 640);
  window.show();
  map_widget->setFocus();
  return app.exec();
}
