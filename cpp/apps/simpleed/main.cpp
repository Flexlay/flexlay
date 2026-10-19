// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/editor_map.hpp"
#include "flexlay/editor_map_widget.hpp"
#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/objmap_sprite_object.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/sprite.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/commands/object_add_command.hpp"
#include "flexlay/tools/objmap_select_tool.hpp"
#include "flexlay/tools/tile_fill_tool.hpp"
#include "flexlay/tools/tile_paint_tool.hpp"
#include "flexlay/supertux/level_loader.hpp"
#include "flexlay/supertux/tileset_loader.hpp"
#include "flexlay/workspace.hpp"


#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <cstdlib>
#include <filesystem>
#include <random>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName("flexlay-simpleed");
  QApplication::setOrganizationName("Flexlay");

  constexpr int tile_size = 32;
  auto tileset = std::make_shared<flexlay::Tileset>(tile_size);
  auto make_tile_image = [tile_size](const flexlay::Color& base, const flexlay::Color& accent) {
    auto pb = flexlay::PixelBuffer::from_color(flexlay::Size(tile_size, tile_size), base);
    for (int y = 0; y < tile_size; ++y) {
      for (int x = 0; x < tile_size; ++x) {
        if (x == 0 || y == 0 || x == tile_size - 1 || y == tile_size - 1) {
          pb.set_pixel(x, y, accent);
        } else if (((x / 4) + (y / 4)) % 2 == 0) {
          pb.set_pixel(x, y, accent);
        }
      }
    }
    return flexlay::Tile(std::move(pb));
  };
  tileset->add_tile(1, make_tile_image(flexlay::Color(70, 130, 180), flexlay::Color(100, 160, 210)));
  tileset->add_tile(2, make_tile_image(flexlay::Color(60, 179, 113), flexlay::Color(90, 200, 140)));
  tileset->add_tile(3, make_tile_image(flexlay::Color(205, 92, 92), flexlay::Color(230, 120, 120)));
  tileset->add_tile(4, make_tile_image(flexlay::Color(238, 221, 130), flexlay::Color(250, 240, 180)));
  tileset->add_tile(5, make_tile_image(flexlay::Color(147, 112, 219), flexlay::Color(180, 150, 240)));

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

  // Demo sprites (solid pixel buffers until tile images are loaded)
  {
    auto pb = flexlay::PixelBuffer::from_color(flexlay::Size(48, 48), flexlay::Color(30, 144, 255, 220));
    // checker accent
    for (int y = 0; y < 48; ++y)
      for (int x = 0; x < 48; ++x)
        if (((x / 8) + (y / 8)) % 2 == 0)
          pb.set_pixel(x, y, flexlay::Color(255, 255, 255, 220));
    object_layer->add_object(std::make_shared<flexlay::ObjMapSpriteObject>(
      flexlay::Sprite(std::move(pb)), flexlay::Pointf(300, 300)));
  }

  auto editor_map = std::make_shared<flexlay::EditorMap>();
  editor_map->set_background_color(flexlay::Color(45, 45, 55));
  editor_map->set_bounding_rect(flexlay::Rectf(
    0, 0, static_cast<float>(map_w * tile_size), static_cast<float>(map_h * tile_size)));
  editor_map->add_layer(tilemap);
  editor_map->add_layer(object_layer);

  auto paint_tool = std::make_shared<flexlay::TilePaintTool>();
  paint_tool->set_tilemap(tilemap);
  paint_tool->set_tile_id(1);

  auto fill_tool = std::make_shared<flexlay::TileFillTool>();
  fill_tool->set_tilemap(tilemap);
  fill_tool->set_tile_id(1);

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
  auto* fill_btn = new QPushButton(QStringLiteral("Flood fill"), sidebar);
  auto* select_btn = new QPushButton(QStringLiteral("Select objects"), sidebar);
  side_layout->addWidget(paint_btn);
  side_layout->addWidget(fill_btn);
  side_layout->addWidget(select_btn);
  auto* add_rect_btn = new QPushButton(QStringLiteral("Add rect"), sidebar);
  side_layout->addWidget(add_rect_btn);

  side_layout->addWidget(new QLabel(QStringLiteral("Tile:"), sidebar));
  auto make_tile_btn = [&](uint32_t id, const QString& label, const QColor& color) {
    auto* btn = new QPushButton(label, sidebar);
    btn->setStyleSheet(QStringLiteral("background-color: %1; min-height: 28px;").arg(color.name()));
    QObject::connect(btn, &QPushButton::clicked, [paint_tool, fill_tool, workspace, id]() {
      paint_tool->set_tile_id(id);
      fill_tool->set_tile_id(id);
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
  QObject::connect(fill_btn, &QPushButton::clicked, [workspace, fill_tool, map_widget]() {
    workspace->set_tool(fill_tool);
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


  auto* file_menu = window.menuBar()->addMenu(QStringLiteral("&File"));
  auto* open_act = file_menu->addAction(QStringLiteral("Open Level…"));
  // SuperTux data root: SUPERTUX_DATADIR env, else empty (placeholders only).
  // Expected layout: $datadir/images/tiles.stts and $datadir/images/...
  const char* datadir_env = std::getenv("SUPERTUX_DATADIR");
  const std::string supertux_datadir = datadir_env ? datadir_env : std::string{};

  QObject::connect(open_act, &QAction::triggered, [&]() {
    const QString path = QFileDialog::getOpenFileName(
      &window, QStringLiteral("Open SuperTux Level"),
      QString(), QStringLiteral("SuperTux levels (*.stlv *.stl);;All (*)"));
    if (path.isEmpty()) {
      return;
    }
    try {
      auto level = flexlay::supertux::load_level_file(path.toStdString());
      std::shared_ptr<flexlay::Tileset> ts;
      int image_tiles = 0;
      if (!supertux_datadir.empty()) {
        const std::string stts = supertux_datadir + "/images/tiles.stts";
        flexlay::supertux::TilesetLoadOptions opts;
        opts.datadir = supertux_datadir;
        opts.tile_size = tile_size;
        opts.load_images = true;
        try {
          auto loaded = flexlay::supertux::load_tileset_file(stts, opts);
          ts = loaded.first;
          for (int id : ts->get_tiles()) {
            if (ts->create(id).has_image()) {
              ++image_tiles;
            }
          }
        } catch (const std::exception& tex) {
          QMessageBox::warning(&window, QStringLiteral("Tileset"),
                               QStringLiteral("Failed to load %1:\n%2")
                                 .arg(QString::fromStdString(stts))
                                 .arg(QString::fromUtf8(tex.what())));
        }
      }
      if (!ts) {
        ts = std::make_shared<flexlay::Tileset>(tile_size);
      }
      // Ensure every id used in the level exists (placeholder if missing).
      for (const auto& tm : level.tilemaps) {
        for (uint32_t id : tm.tiles) {
          if (id != 0 && !ts->has_tile(static_cast<int>(id))) {
            const uint8_t r = static_cast<uint8_t>(40 + (id * 37) % 180);
            const uint8_t g = static_cast<uint8_t>(40 + (id * 73) % 180);
            const uint8_t b = static_cast<uint8_t>(40 + (id * 19) % 180);
            ts->add_tile(static_cast<int>(id), flexlay::Tile(flexlay::Color(r, g, b)));
          }
        }
      }
      auto new_map = flexlay::supertux::editor_map_from_level(level, ts);
      workspace->set_map(new_map);
      for (const auto& layer : new_map->layers()) {
        auto tml = std::dynamic_pointer_cast<flexlay::TilemapLayer>(layer);
        if (tml) {
          paint_tool->set_tilemap(tml);
          fill_tool->set_tilemap(tml);
          break;
        }
      }
      map_widget->update();
      window.statusBar()->showMessage(
        QStringLiteral("Loaded %1 (%2 tilemaps, %3 tiles with images)")
          .arg(QString::fromStdString(level.name))
          .arg(level.tilemaps.size())
          .arg(image_tiles));
    } catch (const std::exception& ex) {
      QMessageBox::warning(&window, QStringLiteral("Open Level"),
                           QString::fromUtf8(ex.what()));
    }
  });

  window.resize(1100, 640);
  window.show();
  map_widget->setFocus();
  return app.exec();
}
