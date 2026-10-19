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
#include "flexlay/tools/place_object_tool.hpp"
#include "flexlay/tools/tile_fill_tool.hpp"
#include "flexlay/tools/tile_paint_tool.hpp"
#include "flexlay/supertux/level_loader.hpp"
#include "flexlay/supertux/level_saver.hpp"
#include "flexlay/supertux/tileset_loader.hpp"
#include "flexlay/workspace.hpp"
#include "flexlay/gui/tile_selector_widget.hpp"
#include "flexlay/gui/layer_selector_widget.hpp"
#include "flexlay/object_prototype.hpp"
#include "flexlay/gui/object_selector_widget.hpp"
#include "flexlay/gui/properties_widget.hpp"
#include "flexlay/tile_brush.hpp"


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
#include <QComboBox>
#include <QDockWidget>
#include <QScrollArea>
#include <QVariant>

#include <memory>
#include <cstdlib>
#include <filesystem>
#include <random>
#include <vector>

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
  {
    auto r = std::make_shared<flexlay::ObjMapRectObject>(
      flexlay::Pointf(64, 64), flexlay::Sizef(64, 48), flexlay::Color(255, 200, 50, 160));
    r->set_name("demo-rect");
    r->properties().set("kind", std::string("rect"));
    r->properties().set("hp", 1);
    object_layer->add_object(r);
  }
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

  auto place_tool = std::make_shared<flexlay::PlaceObjectTool>();
  place_tool->set_object_layer(object_layer);

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

  // Tile selector dock (groups + grid)
  auto* tile_dock = new QDockWidget(QStringLiteral("Tiles"), &window);
  tile_dock->setObjectName(QStringLiteral("tile_selector_dock"));
  auto* tile_dock_inner = new QWidget(tile_dock);
  auto* tile_dock_layout = new QVBoxLayout(tile_dock_inner);
  tile_dock_layout->setContentsMargins(4, 4, 4, 4);
  auto* group_combo = new QComboBox(tile_dock_inner);
  tile_dock_layout->addWidget(group_combo);
  auto* tile_scroll = new QScrollArea(tile_dock_inner);
  tile_scroll->setWidgetResizable(true);
  tile_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  auto* tile_selector = new flexlay::TileSelectorWidget(tile_scroll);
  tile_selector->set_tileset(tileset);
  {
    std::vector<int> demo_ids = {1, 2, 3, 4, 5};
    tile_selector->set_tiles(demo_ids);
    group_combo->addItem(QStringLiteral("Demo"), QVariant::fromValue(0));
  }
  tile_scroll->setWidget(tile_selector);
  tile_dock_layout->addWidget(tile_scroll, 1);
  tile_dock->setWidget(tile_dock_inner);
  window.addDockWidget(Qt::RightDockWidgetArea, tile_dock);


  // Layers dock
  auto* layer_dock = new QDockWidget(QStringLiteral("Layers"), &window);
  layer_dock->setObjectName(QStringLiteral("layer_selector_dock"));
  auto* layer_selector = new flexlay::LayerSelectorWidget(layer_dock);
  layer_selector->set_map(editor_map);
  layer_dock->setWidget(layer_selector);
  window.addDockWidget(Qt::LeftDockWidgetArea, layer_dock);

  QObject::connect(layer_selector, &flexlay::LayerSelectorWidget::tilemap_activated,
                   [paint_tool, fill_tool, workspace, map_widget](std::shared_ptr<flexlay::TilemapLayer> tm) {
                     paint_tool->set_tilemap(tm);
                     fill_tool->set_tilemap(tm);
                     workspace->set_tool(paint_tool);
                     map_widget->update();
                   });
  QObject::connect(layer_selector, &flexlay::LayerSelectorWidget::object_layer_activated,
                   [select_tool, place_tool, workspace, map_widget](std::shared_ptr<flexlay::ObjectLayer> ol) {
                     select_tool->set_object_layer(ol);
          place_tool->set_object_layer(ol);
                     workspace->set_tool(select_tool);
                     map_widget->update();
                   });
  QObject::connect(layer_selector, &flexlay::LayerSelectorWidget::layer_visibility_changed,
                   [map_widget, editor_map](int, bool) {
                     editor_map->bump_serial();
                     map_widget->update();
                   });


  // Properties dock
  auto* props_dock = new QDockWidget(QStringLiteral("Properties"), &window);
  props_dock->setObjectName(QStringLiteral("properties_dock"));
  auto* props_widget = new flexlay::PropertiesWidget(props_dock);
  props_dock->setWidget(props_widget);
  window.addDockWidget(Qt::RightDockWidgetArea, props_dock);

  // Object palette dock
  auto* obj_dock = new QDockWidget(QStringLiteral("Objects"), &window);
  obj_dock->setObjectName(QStringLiteral("object_selector_dock"));
  auto* obj_inner = new QWidget(obj_dock);
  auto* obj_layout = new QVBoxLayout(obj_inner);
  auto* obj_selector = new flexlay::ObjectSelectorWidget(obj_inner);
  obj_selector->set_prototypes(flexlay::default_object_prototypes());
  auto* add_obj_btn = new QPushButton(QStringLiteral("Add at view center"), obj_inner);
  obj_layout->addWidget(obj_selector, 1);
  obj_layout->addWidget(add_obj_btn);
  obj_dock->setWidget(obj_inner);
  window.addDockWidget(Qt::RightDockWidgetArea, obj_dock);
  window.tabifyDockWidget(tile_dock, props_dock);
  window.tabifyDockWidget(props_dock, obj_dock);
  tile_dock->raise();

  auto refresh_properties_from_selection = [props_widget, select_tool]() {
    auto layer = select_tool->object_layer();
    if (!layer) {
      props_widget->clear();
      return;
    }
    auto selected = layer->selected_objects();
    if (selected.size() == 1) {
      props_widget->set_object(selected.front());
    } else {
      props_widget->clear();
    }
  };
  select_tool->on_selection_changed = [refresh_properties_from_selection, map_widget]() {
    refresh_properties_from_selection();
    map_widget->update();
  };
  QObject::connect(props_widget, &flexlay::PropertiesWidget::properties_changed,
                   [map_widget]() { map_widget->update(); });

  auto activate_place_tool = [place_tool, workspace, map_widget, select_tool, props_widget](
                               const flexlay::ObjectPrototype& proto) {
    place_tool->set_prototype(proto);
    place_tool->on_placed = [props_widget, map_widget, select_tool, workspace](
                              std::shared_ptr<flexlay::ObjMapObject> obj) {
      props_widget->set_object(obj);
      workspace->set_tool(select_tool);
      map_widget->update();
    };
    place_tool->on_cancelled = [workspace, select_tool, map_widget]() {
      workspace->set_tool(select_tool);
      map_widget->update();
    };
    workspace->set_tool(place_tool);
    map_widget->setFocus();
    map_widget->update();
  };

  QObject::connect(obj_selector, &flexlay::ObjectSelectorWidget::prototype_activated,
                   activate_place_tool);
  QObject::connect(add_obj_btn, &QPushButton::clicked, [obj_selector, activate_place_tool]() {
    if (const auto* p = obj_selector->current_prototype()) {
      activate_place_tool(*p);
    }
  });


  // Keep last loaded tilegroups for combo switching
  auto tilegroups = std::make_shared<std::vector<flexlay::supertux::TileGroupInfo>>();
  auto current_tileset = std::make_shared<std::shared_ptr<flexlay::Tileset>>(tileset);

  auto apply_tilegroup = [tile_selector, group_combo, tilegroups, current_tileset](int idx) {
    if (!*current_tileset) {
      return;
    }
    tile_selector->set_tileset(*current_tileset);
    if (idx < 0 || tilegroups->empty()) {
      // all tiles from tileset
      tile_selector->set_tiles((*current_tileset)->get_tiles());
      return;
    }
    if (idx >= static_cast<int>(tilegroups->size())) {
      tile_selector->set_tiles((*current_tileset)->get_tiles());
      return;
    }
    tile_selector->set_tiles((*tilegroups)[static_cast<size_t>(idx)].tiles);
  };

  QObject::connect(group_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   apply_tilegroup);

  QObject::connect(tile_selector, &flexlay::TileSelectorWidget::tile_selected,
                   [paint_tool, fill_tool, workspace](int id) {
                     if (id <= 0) {
                       return;
                     }
                     paint_tool->set_tile_id(static_cast<uint32_t>(id));
                     fill_tool->set_tile_id(static_cast<uint32_t>(id));
                     workspace->set_tool(paint_tool);
                   });

  QObject::connect(tile_selector, &flexlay::TileSelectorWidget::brush_selected,
                   [paint_tool, workspace](int w, int h, const std::vector<uint32_t>& ids) {
                     flexlay::TileBrush brush(w, h);
                     for (int y = 0; y < h; ++y) {
                       for (int x = 0; x < w; ++x) {
                         brush.put(x, y, ids[static_cast<size_t>(y * w + x)]);
                       }
                     }
                     brush.set_opaque(false);
                     paint_tool->set_brush(brush);
                     workspace->set_tool(paint_tool);
                   });
  window.setCentralWidget(central);

  QObject::connect(paint_btn, &QPushButton::clicked, [workspace, paint_tool, map_widget]() {
    workspace->set_tool(paint_tool);
    map_widget->setFocus();
  });
  QObject::connect(fill_btn, &QPushButton::clicked, [workspace, fill_tool, map_widget]() {
    workspace->set_tool(fill_tool);
    map_widget->setFocus();
  });
  QObject::connect(select_btn, &QPushButton::clicked, [workspace, select_tool, map_widget, refresh_properties_from_selection]() {
    workspace->set_tool(select_tool);
    refresh_properties_from_selection();
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
  auto last_level_name = std::make_shared<std::string>("unnamed");
  auto last_level_meta = std::make_shared<flexlay::supertux::LoadedLevel>();

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
      flexlay::supertux::LoadedTilesetMeta meta;
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
          meta = loaded.second;
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
      *last_level_name = level.name.empty() ? "unnamed" : level.name;
      *last_level_meta = level; // keep author/license/tileset for save
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
      *current_tileset = ts;
      tile_selector->set_tileset(ts);
      layer_selector->set_map(new_map);
      // Rebind object layer for tools/properties if present
      for (const auto& layer : new_map->layers()) {
        if (auto ol = std::dynamic_pointer_cast<flexlay::ObjectLayer>(layer)) {
          select_tool->set_object_layer(ol);
          place_tool->set_object_layer(ol);
          break;
        }
      }
      props_widget->clear();
      *tilegroups = meta.tilegroups;
      group_combo->blockSignals(true);
      group_combo->clear();
      if (tilegroups->empty()) {
        group_combo->addItem(QStringLiteral("All tiles"));
        tile_selector->set_tiles(ts->get_tiles());
      } else {
        for (const auto& g : *tilegroups) {
          group_combo->addItem(QString::fromStdString(g.name));
        }
        group_combo->addItem(QStringLiteral("All tiles"));
        group_combo->setCurrentIndex(0);
        tile_selector->set_tiles((*tilegroups)[0].tiles);
      }
      group_combo->blockSignals(false);
      map_widget->update();
      window.statusBar()->showMessage(
        QStringLiteral("Loaded %1 (%2 tilemaps, %3 objects, %4 tiles w/ images, %5 groups)")
          .arg(QString::fromStdString(level.name))
          .arg(level.tilemaps.size())
          .arg(level.objects.size())
          .arg(image_tiles)
          .arg(tilegroups->size()));
    } catch (const std::exception& ex) {
      QMessageBox::warning(&window, QStringLiteral("Open Level"),
                           QString::fromUtf8(ex.what()));
    }
  });


  auto* save_act = file_menu->addAction(QStringLiteral("Save Level As…"));
  QObject::connect(save_act, &QAction::triggered, [&]() {
    auto map = workspace->get_map();
    if (!map) {
      return;
    }
    const QString path = QFileDialog::getSaveFileName(
      &window, QStringLiteral("Save SuperTux Level"),
      QString::fromStdString(*last_level_name + ".stlv"),
      QStringLiteral("SuperTux levels (*.stlv *.stl);;All (*)"));
    if (path.isEmpty()) {
      return;
    }
    try {
      auto level = flexlay::supertux::level_from_editor_map(
        *map, *last_level_name, "main",
        last_level_meta->version > 0 ? last_level_meta->version : 4,
        last_level_meta.get());
      flexlay::supertux::save_level_file(level, path.toStdString());
      map->set_modified(false);
      window.statusBar()->showMessage(
        QStringLiteral("Saved %1 (%2 tilemaps, %3 objects)")
          .arg(path)
          .arg(level.tilemaps.size())
          .arg(level.objects.size()));
    } catch (const std::exception& ex) {
      QMessageBox::warning(&window, QStringLiteral("Save Level"),
                           QString::fromUtf8(ex.what()));
    }
  });

  window.resize(1100, 640);
  window.show();
  map_widget->setFocus();
  return app.exec();
}
