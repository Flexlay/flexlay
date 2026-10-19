// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map.hpp"
#include "flexlay/editor_map_widget.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/commands/object_add_command.hpp"
#include "flexlay/tools/objmap_select_tool.hpp"
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

  auto object_layer = std::make_shared<flexlay::ObjectLayer>();
  object_layer->add_object(std::make_shared<flexlay::ObjMapRectObject>(
    flexlay::Pointf(64, 64), flexlay::Sizef(64, 48), flexlay::Color(255, 200, 50, 160)));
  object_layer->add_object(std::make_shared<flexlay::ObjMapRectObject>(
    flexlay::Pointf(200, 120), flexlay::Sizef(80, 80), flexlay::Color(50, 200, 255, 160)));
  object_layer->add_object(std::make_shared<flexlay::ObjMapRectObject>(
    flexlay::Pointf(400, 200), flexlay::Sizef(96, 40), flexlay::Color(255, 80, 120, 160)));

  auto editor_map = std::make_shared<flexlay::EditorMap>();
  editor_map->set_background_color(flexlay::Color(45, 45, 55));
  editor_map->set_bounding_rect(flexlay::Rectf(
    0, 0, static_cast<float>(map_w * tile_size), static_cast<float>(map_h * tile_size)));
  editor_map->add_layer(tilemap);
  editor_map->add_layer(object_layer);

  auto paint_tool = std::make_shared<flexlay::TilePaintTool>();
  paint_tool->set_tilemap(tilemap);
  paint_tool->set_tile_id(1);

  auto select_tool = std::make_shared<flexlay::ObjMapSelectTool>();
  select_tool->set_object_layer(object_layer);

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

  side_layout->addWidget(new QLabel(QStringLiteral("Tool:"), sidebar));
  auto* paint_btn = new QPushButton(QStringLiteral("Paint tiles"), sidebar);
  auto* select_btn = new QPushButton(QStringLiteral("Select objects"), sidebar);
  side_layout->addWidget(paint_btn);
  side_layout->addWidget(select_btn);
  auto* add_rect_btn = new QPushButton(QStringLiteral("Add rect"), sidebar);
  side_layout->addWidget(add_rect_btn);

  side_layout->addWidget(new QLabel(QStringLiteral("Tile:"), sidebar));
  auto make_tile_btn = [&](uint32_t id, const QString& label, const QColor& color) {
    auto* btn = new QPushButton(label, sidebar);
    btn->setStyleSheet(QStringLiteral("background-color: %1; min-height: 28px;").arg(color.name()));
    QObject::connect(btn, &QPushButton::clicked, [paint_tool, workspace, id]() {
      paint_tool->set_tile_id(id);
      workspace->set_tool(paint_tool);
    });
    side_layout->addWidget(btn);
  };
  make_tile_btn(0, QStringLiteral("0 Erase"), QColor(40, 40, 40));
  make_tile_btn(1, QStringLiteral("1 Blue"), QColor(70, 130, 180));
  make_tile_btn(2, QStringLiteral("2 Green"), QColor(60, 179, 113));
  make_tile_btn(3, QStringLiteral("3 Red"), QColor(205, 92, 92));
  make_tile_btn(4, QStringLiteral("4 Yellow"), QColor(238, 221, 130));
  make_tile_btn(5, QStringLiteral("5 Purple"), QColor(147, 112, 219));

  side_layout->addWidget(new QLabel(QStringLiteral("Brush size:"), sidebar));
  for (int sz : {1, 2, 3, 5}) {
    auto* btn = new QPushButton(QString::number(sz), sidebar);
    QObject::connect(btn, &QPushButton::clicked, [paint_tool, workspace, sz]() {
      paint_tool->set_brush_size(sz);
      workspace->set_tool(paint_tool);
    });
    side_layout->addWidget(btn);
  }
  side_layout->addStretch(1);

  auto* map_widget = new flexlay::EditorMapWidget(central);
  map_widget->set_workspace(workspace);
  layout->addWidget(sidebar, 0);
  layout->addWidget(map_widget, 1);
  window.setCentralWidget(central);

  QObject::connect(paint_btn, &QPushButton::clicked, [workspace, paint_tool, map_widget]() {
    workspace->set_tool(paint_tool);
    map_widget->setFocus();
  });
  QObject::connect(select_btn, &QPushButton::clicked, [workspace, select_tool, map_widget]() {
    workspace->set_tool(select_tool);
    map_widget->setFocus();
  });

  QObject::connect(add_rect_btn, &QPushButton::clicked, [workspace, object_layer, select_tool, editor_map, map_widget]() {
    workspace->set_tool(select_tool);
    auto rect = std::make_shared<flexlay::ObjMapRectObject>(
      flexlay::Pointf(100, 100), flexlay::Sizef(64, 48), flexlay::Color(180, 255, 100, 180));
    auto cmd = std::make_shared<flexlay::ObjectAddCommand>(object_layer);
    cmd->add_object(rect);
    // apply immediately then push (execute will no-op if we mark applied — simpler: just execute)
    editor_map->execute(cmd);
    object_layer->clear_selection();
    rect->set_selected(true);
    map_widget->update();
    map_widget->setFocus();
  });


  auto update_status = [&]() {
    window.statusBar()->showMessage(
      QStringLiteral("LMB paint | RMB-drag copy brush | MMB pan | Wheel | Ctrl+Z/Y | brush=%1x%2 tile=%3 | undo=%4")
        .arg(paint_tool->brush().width())
        .arg(paint_tool->brush().height())
        .arg(paint_tool->tile_id())
        .arg(editor_map->undo_stack_size()));
  };
  QObject::connect(map_widget, &flexlay::EditorMapWidget::map_changed, update_status);
  update_status();

  window.resize(1100, 640);
  window.show();
  map_widget->setFocus();
  return app.exec();
}
