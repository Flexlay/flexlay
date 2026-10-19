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
#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QToolBar>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QDockWidget>
#include <QScrollArea>
#include <QVariant>

#include <memory>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

struct CliOptions
{
  std::string datadir;
  std::string level;
  bool verbose = false;
  bool help = false;
  bool demo = true;
};

void print_help(const char* argv0)
{
  const char* name = argv0 ? argv0 : "supertux-editor";
  std::cerr
    << "Usage: " << name << " [options] [level.stlv]\n"
    << "\n"
    << "  SuperTux level editor (C++ / Flexlay).\n"
    << "  Without --level, starts with a DEMO map — not a SuperTux level.\n"
    << "  SUPERTUX_DATADIR alone does NOT open a level; use --level or File→Open.\n"
    << "\n"
    << "Options:\n"
    << "  -h, --help           Show this help\n"
    << "  -v, --verbose        Extra debug on stderr\n"
    << "  -d, --datadir DIR    SuperTux *data* root (must contain images/tiles.stts)\n"
    << "                       Default: $SUPERTUX_DATADIR\n"
    << "  -l, --level FILE     Open this .stlv/.stl on startup\n"
    << "  --no-demo            Start with an empty map instead of demo tiles\n"
    << "\n"
    << "Example:\n"
    << "  " << name << " -d /path/to/supertux/data \\\n"
    << "    -l /path/to/supertux/data/levels/world1/1\\ -\\ Welcome\\ to\\ Antarctica.stlv -v\n";
}

CliOptions parse_cli(int argc, char* argv[])
{
  CliOptions opt;
  if (const char* env = std::getenv("SUPERTUX_DATADIR")) {
    opt.datadir = env;
  }
  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i] ? argv[i] : "";
    if (a == "-h" || a == "--help") {
      opt.help = true;
    } else if (a == "-v" || a == "--verbose") {
      opt.verbose = true;
    } else if (a == "--no-demo") {
      opt.demo = false;
    } else if ((a == "-d" || a == "--datadir") && i + 1 < argc) {
      opt.datadir = argv[++i];
    } else if ((a == "-l" || a == "--level") && i + 1 < argc) {
      opt.level = argv[++i];
      opt.demo = false;
    } else if (!a.empty() && a[0] != '-') {
      opt.level = a;
      opt.demo = false;
    } else {
      std::cerr << "Unknown option: " << a << " (try --help)\n";
      opt.help = true;
    }
  }
  return opt;
}

std::string resolve_datadir(std::string dir, bool verbose)
{
  namespace fs = std::filesystem;
  if (dir.empty()) {
    return dir;
  }
  while (!dir.empty() && (dir.back() == '/' || dir.back() == '\\')) {
    dir.pop_back();
  }
  const fs::path p(dir);
  if (fs::is_regular_file(p / "images" / "tiles.stts")) {
    if (verbose) {
      std::cerr << "[flexlay] datadir OK: " << dir << " (images/tiles.stts found)\n";
    }
    return dir;
  }
  if (fs::is_regular_file(p / "data" / "images" / "tiles.stts")) {
    const std::string fixed = (p / "data").string();
    std::cerr << "[flexlay] datadir: adjusted to " << fixed
              << " (found data/images/tiles.stts under what you passed)\n";
    return fixed;
  }
  std::cerr << "[flexlay] WARNING: \"" << dir
            << "\" has no images/tiles.stts — tile images will be placeholders.\n"
            << "  Pass the SuperTux *data* directory, e.g. .../supertux/data\n";
  return dir;
}

void log_startup(const CliOptions& opt)
{
  std::cerr << "[flexlay] SuperTux Editor C++ (flexlay-simpleed binary)\n";
  if (opt.datadir.empty()) {
    std::cerr << "[flexlay] datadir: (unset)  use -d DIR or SUPERTUX_DATADIR\n";
  } else {
    std::cerr << "[flexlay] datadir: " << opt.datadir << "\n";
  }
  if (!opt.level.empty()) {
    std::cerr << "[flexlay] level:   " << opt.level << "\n";
  } else if (opt.demo) {
    std::cerr << "[flexlay] map:     DEMO (random tiles) — not SuperTux content\n";
  } else {
    std::cerr << "[flexlay] map:     empty (--no-demo)\n";
  }
}

} // namespace

int main(int argc, char* argv[])
{
  CliOptions options = parse_cli(argc, argv);
  if (options.help) {
    print_help(argc > 0 ? argv[0] : nullptr);
    return 0;
  }
  options.datadir = resolve_datadir(std::move(options.datadir), options.verbose);
  log_startup(options);

  QApplication app(argc, argv);
  QApplication::setApplicationName("supertux-editor");
  QApplication::setApplicationDisplayName("SuperTux Editor (C++)");
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

  if (options.demo) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 5);
    for (int y = 0; y < map_h; ++y) {
      for (int x = 0; x < map_w; ++x) {
        int id = dist(rng);
        if ((x + y) % 7 == 0) id = 0;
        tilemap->field().put(x, y, static_cast<uint32_t>(id));
      }
    }
  }

  auto object_layer = std::make_shared<flexlay::ObjectLayer>();
  object_layer->set_name("objects");
  if (options.demo) {
    auto r = std::make_shared<flexlay::ObjMapRectObject>(
      flexlay::Pointf(64, 64), flexlay::Sizef(64, 48), flexlay::Color(255, 200, 50, 160));
    r->set_name("demo-rect");
    r->properties().set("kind", std::string("rect"));
    r->properties().set("hp", 1);
    object_layer->add_object(r);
    object_layer->add_object(std::make_shared<flexlay::ObjMapRectObject>(
      flexlay::Pointf(200, 120), flexlay::Sizef(80, 80), flexlay::Color(50, 200, 255, 160)));
    object_layer->add_object(std::make_shared<flexlay::ObjMapRectObject>(
      flexlay::Pointf(400, 200), flexlay::Sizef(96, 40), flexlay::Color(255, 80, 120, 160)));
    auto pb = flexlay::PixelBuffer::from_color(flexlay::Size(48, 48), flexlay::Color(30, 144, 255, 220));
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
  // datadir set after env is read (see below)

  auto workspace = std::make_shared<flexlay::Workspace>();
  workspace->set_map(editor_map);
  workspace->set_tool(paint_tool);
  workspace->state().set_size(960, 600);
  workspace->state().set_pos(flexlay::Pointf(static_cast<float>(map_w * tile_size) / 2.0f,
                                             static_cast<float>(map_h * tile_size) / 2.0f));

  QMainWindow window;
  // Title: demo vs datadir (matches Python "SuperTux" naming more closely)
  {
    QString title = QStringLiteral("SuperTux Editor");
    if (options.demo) {
      title += QStringLiteral(" — Demo");
    }
    if (!options.datadir.empty()) {
      title += QStringLiteral(" [%1]").arg(QString::fromStdString(options.datadir));
    }
    window.setWindowTitle(title);
  }

  auto* central = new QWidget(&window);
  auto* layout = new QHBoxLayout(central);
  layout->setContentsMargins(0, 0, 0, 0);
  auto* map_widget = new flexlay::EditorMapWidget(central);
  map_widget->set_workspace(workspace);
  layout->addWidget(map_widget, 1);

  // Toolbar — similar roles to Python SuperTuxButtonPanel / toolbox
  auto* toolbar = window.addToolBar(QStringLiteral("Main"));
  toolbar->setObjectName(QStringLiteral("main_toolbar"));
  toolbar->setMovable(false);
  auto* tool_group = new QActionGroup(&window);
  tool_group->setExclusive(true);

  auto* paint_act = toolbar->addAction(QStringLiteral("Paint"));
  paint_act->setCheckable(true);
  paint_act->setChecked(true);
  paint_act->setShortcut(QKeySequence(QStringLiteral("P")));
  paint_act->setToolTip(QStringLiteral("Paint tiles (P)"));
  tool_group->addAction(paint_act);

  auto* fill_act = toolbar->addAction(QStringLiteral("Fill"));
  fill_act->setCheckable(true);
  fill_act->setShortcut(QKeySequence(QStringLiteral("F")));
  fill_act->setToolTip(QStringLiteral("Flood fill (F)"));
  tool_group->addAction(fill_act);

  auto* select_act = toolbar->addAction(QStringLiteral("Objects"));
  select_act->setCheckable(true);
  select_act->setShortcut(QKeySequence(QStringLiteral("O")));
  select_act->setToolTip(QStringLiteral("Select / move objects (O)"));
  tool_group->addAction(select_act);

  toolbar->addSeparator();
  auto* grid_act = toolbar->addAction(QStringLiteral("Grid"));
  grid_act->setCheckable(true);
  grid_act->setChecked(true);
  grid_act->setShortcut(QKeySequence(QStringLiteral("G")));
  grid_act->setToolTip(QStringLiteral("Toggle tile grid (G)"));

  toolbar->addSeparator();
  auto* zoom_in_act = toolbar->addAction(QStringLiteral("Zoom+"));
  auto* zoom_out_act = toolbar->addAction(QStringLiteral("Zoom-"));
  auto* zoom_1_act = toolbar->addAction(QStringLiteral("1:1"));
  auto* zoom_fit_act = toolbar->addAction(QStringLiteral("Fit"));
  zoom_1_act->setToolTip(QStringLiteral("Zoom 100%"));
  zoom_fit_act->setToolTip(QStringLiteral("Zoom to fit level"));

  toolbar->addSeparator();
  for (int sz : {1, 2, 3, 5}) {
    auto* brush_act = toolbar->addAction(QStringLiteral("Brush %1").arg(sz));
    QObject::connect(brush_act, &QAction::triggered, [paint_tool, workspace, paint_act, sz]() {
      paint_tool->set_brush_size(sz);
      workspace->set_tool(paint_tool);
      paint_act->setChecked(true);
    });
  }

  editor_map->set_draw_grid(true);
  editor_map->set_background_color(flexlay::Color(100, 150, 200));

  QObject::connect(paint_act, &QAction::triggered, [workspace, paint_tool, map_widget]() {
    workspace->set_tool(paint_tool);
    map_widget->setFocus();
  });
  QObject::connect(fill_act, &QAction::triggered, [workspace, fill_tool, map_widget]() {
    workspace->set_tool(fill_tool);
    map_widget->setFocus();
  });
  QObject::connect(select_act, &QAction::triggered, [workspace, select_tool, map_widget]() {
    workspace->set_tool(select_tool);
    map_widget->setFocus();
  });
  QObject::connect(grid_act, &QAction::toggled, [workspace, map_widget](bool on) {
    if (auto* map = workspace->map()) {
      map->set_draw_grid(on);
      map_widget->update();
    }
  });
  auto apply_zoom = [workspace, map_widget](float z) {
    workspace->state().set_zoom(z);
    map_widget->update();
  };
  QObject::connect(zoom_in_act, &QAction::triggered, [workspace, map_widget]() {
    const float z = std::min(16.0f, workspace->state().get_zoom() * 1.25f);
    workspace->state().set_zoom(z);
    map_widget->update();
  });
  QObject::connect(zoom_out_act, &QAction::triggered, [workspace, map_widget]() {
    const float z = std::max(0.1f, workspace->state().get_zoom() / 1.25f);
    workspace->state().set_zoom(z);
    map_widget->update();
  });
  QObject::connect(zoom_1_act, &QAction::triggered, [apply_zoom]() { apply_zoom(1.0f); });
  QObject::connect(zoom_fit_act, &QAction::triggered, [workspace, map_widget]() {
    if (auto* map = workspace->map()) {
      const auto br = map->bounding_rect();
      const float mw = std::max(1.0f, br.width());
      const float mh = std::max(1.0f, br.height());
      const float zw = static_cast<float>(map_widget->width()) / mw;
      const float zh = static_cast<float>(map_widget->height()) / mh;
      const float z = std::clamp(std::min(zw, zh) * 0.9f, 0.1f, 16.0f);
      workspace->state().set_zoom(z);
      workspace->state().set_pos(flexlay::Pointf(br.left + mw * 0.5f, br.top + mh * 0.5f));
      map_widget->update();
    }
  });

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
  // Sector combo is in the Objects sidebar


  // Sector selector (multi-sector levels)
  auto* sector_combo = new QComboBox(&window);
  sector_combo->setToolTip(QStringLiteral("Active SuperTux sector"));
  sector_combo->setEnabled(false);
  sector_combo->addItem(QStringLiteral("main"));

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
  props_widget->set_commit_handler([workspace, map_widget](flexlay::CommandPtr cmd) {
    if (auto* map = workspace->map()) {
      map->execute(std::move(cmd));
    }
    map_widget->update();
  });
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
  obj_layout->addWidget(new QLabel(QStringLiteral("Sector:"), obj_inner));
  // reparent sector combo into dock
  sector_combo->setParent(obj_inner);
  obj_layout->addWidget(sector_combo);
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


  auto update_status = [&]() {
    window.statusBar()->showMessage(
      QStringLiteral("LMB paint | RMB-drag copy brush | MMB pan | Wheel | Ctrl+Z/Y | brush=%1x%2 tile=%3 | undo=%4")
        .arg(paint_tool->brush().width())
        .arg(paint_tool->brush().height())
        .arg(paint_tool->tile_id())
        .arg(editor_map->undo_stack_size()));
  };
  QObject::connect(map_widget, &flexlay::EditorMapWidget::map_changed, update_status);
  QObject::connect(map_widget, &flexlay::EditorMapWidget::map_changed, [props_widget]() {
    // After undo/redo, rebuild form so fields match object state
    props_widget->refresh();
  });
  update_status();


  auto* file_menu = window.menuBar()->addMenu(QStringLiteral("&File"));
  auto* open_act = file_menu->addAction(QStringLiteral("Open…"));
  open_act->setShortcut(QKeySequence::Open);
  // Shared so Open Level / sector switch see the same resolved path
  auto supertux_datadir = std::make_shared<std::string>(options.datadir);
  place_tool->set_datadir(*supertux_datadir);
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
      if (options.verbose) {
        std::cerr << "[flexlay] opening level: " << path.toStdString() << "\n";
        std::cerr << "[flexlay] using datadir: " << (*supertux_datadir) << "\n";
      }
      auto level = flexlay::supertux::load_level_file(path.toStdString());
      std::shared_ptr<flexlay::Tileset> ts;
      flexlay::supertux::LoadedTilesetMeta meta;
      int image_tiles = 0;
      if (!supertux_datadir->empty()) {
        const std::string stts = *supertux_datadir + "/images/tiles.stts";
        flexlay::supertux::TilesetLoadOptions opts;
        opts.datadir = *supertux_datadir;
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
      for (const auto& tm : level.tilemaps()) {
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
      auto new_map = flexlay::supertux::editor_map_from_level(level, ts, *supertux_datadir);
      workspace->set_map(new_map);
      // Prefer solid tilemap for paint (matches Python get_some_solid_tilemap)
      {
        std::shared_ptr<flexlay::TilemapLayer> solid;
        std::shared_ptr<flexlay::TilemapLayer> any_tm;
        for (const auto& layer : new_map->layers()) {
          if (auto tml = std::dynamic_pointer_cast<flexlay::TilemapLayer>(layer)) {
            if (!any_tm) any_tm = tml;
            if (tml->get_name().find("[solid]") != std::string::npos) {
              solid = tml;
              break;
            }
          }
        }
        auto target = solid ? solid : any_tm;
        if (target) {
          paint_tool->set_tilemap(target);
          fill_tool->set_tilemap(target);
        }
      }
      paint_act->setChecked(true);
      workspace->set_tool(paint_tool);
      grid_act->setChecked(new_map->draw_grid());
      // Zoom to fit like Python zoom-fit after load
      {
        const auto br = new_map->bounding_rect();
        const float mw = std::max(1.0f, br.width());
        const float mh = std::max(1.0f, br.height());
        const float zw = static_cast<float>(std::max(map_widget->width(), 200)) / mw;
        const float zh = static_cast<float>(std::max(map_widget->height(), 200)) / mh;
        const float z = std::clamp(std::min(zw, zh) * 0.92f, 0.15f, 4.0f);
        workspace->state().set_zoom(z);
        workspace->state().set_pos(flexlay::Pointf(br.left + mw * 0.5f, br.top + mh * 0.5f));
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
      window.setWindowTitle(
        QStringLiteral("SuperTux Editor (C++) — %1")
          .arg(QString::fromStdString(level.name.empty() ? path.toStdString() : level.name)));
      // Populate sector combo
      sector_combo->blockSignals(true);
      sector_combo->clear();
      for (const auto& sn : level.sector_names()) {
        sector_combo->addItem(QString::fromStdString(sn));
      }
      sector_combo->setCurrentIndex(static_cast<int>(level.active_sector));
      sector_combo->setEnabled(level.sectors.size() > 1);
      sector_combo->blockSignals(false);

      window.statusBar()->showMessage(
        QStringLiteral("Loaded %1 [sector %2] (%3 tilemaps, %4 objects, %5 tiles w/ images, %6 groups, %7 sectors)")
          .arg(QString::fromStdString(level.name))
          .arg(QString::fromStdString(level.sector_name()))
          .arg(level.tilemaps().size())
          .arg(level.objects().size())
          .arg(image_tiles)
          .arg(tilegroups->size())
          .arg(level.sectors.size()));
    } catch (const std::exception& ex) {
      QMessageBox::warning(&window, QStringLiteral("Open Level"),
                           QString::fromUtf8(ex.what()));
    }
  });


  auto* save_act = file_menu->addAction(QStringLiteral("Save As…"));
  save_act->setShortcut(QKeySequence::SaveAs);
  file_menu->addSeparator();
  auto* quit_act = file_menu->addAction(QStringLiteral("Quit"));
  quit_act->setShortcut(QKeySequence::Quit);
  QObject::connect(quit_act, &QAction::triggered, &window, &QMainWindow::close);

  auto* edit_menu = window.menuBar()->addMenu(QStringLiteral("&Edit"));
  auto* undo_act = edit_menu->addAction(QStringLiteral("Undo"));
  undo_act->setShortcut(QKeySequence::Undo);
  auto* redo_act = edit_menu->addAction(QStringLiteral("Redo"));
  redo_act->setShortcut(QKeySequence::Redo);
  QObject::connect(undo_act, &QAction::triggered, [workspace, map_widget]() {
    if (auto* map = workspace->map()) { map->undo(); map_widget->update(); }
  });
  QObject::connect(redo_act, &QAction::triggered, [workspace, map_widget]() {
    if (auto* map = workspace->map()) { map->redo(); map_widget->update(); }
  });

  auto* view_menu = window.menuBar()->addMenu(QStringLiteral("&View"));
  view_menu->addAction(grid_act);
  view_menu->addSeparator();
  auto add_zoom = [&](const QString& label, float z) {
    auto* a = view_menu->addAction(label);
    QObject::connect(a, &QAction::triggered, [workspace, map_widget, z]() {
      workspace->state().set_zoom(z);
      map_widget->update();
    });
  };
  add_zoom(QStringLiteral("1:4 (25%)"), 0.25f);
  add_zoom(QStringLiteral("1:2 (50%)"), 0.5f);
  add_zoom(QStringLiteral("1:1 (100%)"), 1.0f);
  add_zoom(QStringLiteral("2:1 (200%)"), 2.0f);
  add_zoom(QStringLiteral("4:1 (400%)"), 4.0f);
  view_menu->addSeparator();
  view_menu->addAction(zoom_fit_act);

  auto* help_menu = window.menuBar()->addMenu(QStringLiteral("&Help"));
  auto* about_act = help_menu->addAction(QStringLiteral("About"));
  QObject::connect(about_act, &QAction::triggered, [&window]() {
    QMessageBox::about(&window, QStringLiteral("SuperTux Editor"),
      QStringLiteral("SuperTux level editor (C++ / Flexlay).\n"
                     "Open a .stlv with File→Open. Set datadir with -d or SUPERTUX_DATADIR.\n"
                     "Tools: Paint (P), Fill (F), Objects (O), Grid (G)."));
  });
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
      const std::string sec = last_level_meta->sector_name();
      auto level = flexlay::supertux::level_from_editor_map(
        *map, *last_level_name, sec.empty() ? "main" : sec,
        last_level_meta->version > 0 ? last_level_meta->version : 4,
        last_level_meta.get());
      *last_level_meta = level;
      flexlay::supertux::save_level_file(level, path.toStdString());
      map->set_modified(false);
      window.statusBar()->showMessage(
        QStringLiteral("Saved %1 (%2 sectors, active %3: %4 tilemaps, %5 objects)")
          .arg(path)
          .arg(level.sectors.size())
          .arg(QString::fromStdString(level.sector_name()))
          .arg(level.tilemaps().size())
          .arg(level.objects().size()));
    } catch (const std::exception& ex) {
      QMessageBox::warning(&window, QStringLiteral("Save Level"),
                           QString::fromUtf8(ex.what()));
    }
  });


  QObject::connect(sector_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [&, sector_combo](int idx) {
    if (idx < 0 || !last_level_meta || last_level_meta->sectors.empty()) {
      return;
    }
    if (static_cast<size_t>(idx) >= last_level_meta->sectors.size()) {
      return;
    }
    // Persist current map into the previously active sector before switching
    if (auto map = workspace->get_map()) {
      const std::string prev = last_level_meta->sector_name();
      *last_level_meta = flexlay::supertux::level_from_editor_map(
        *map, *last_level_name, prev,
        last_level_meta->version > 0 ? last_level_meta->version : 4,
        last_level_meta.get());
    }
    last_level_meta->active_sector = static_cast<size_t>(idx);
    auto ts = *current_tileset;
    if (!ts) {
      ts = std::make_shared<flexlay::Tileset>(32);
    }
    auto new_map = flexlay::supertux::editor_map_from_level(*last_level_meta, ts, *supertux_datadir);
    workspace->set_map(new_map);
    for (const auto& layer : new_map->layers()) {
      if (auto tml = std::dynamic_pointer_cast<flexlay::TilemapLayer>(layer)) {
        paint_tool->set_tilemap(tml);
        fill_tool->set_tilemap(tml);
        break;
      }
    }
    for (const auto& layer : new_map->layers()) {
      if (auto ol = std::dynamic_pointer_cast<flexlay::ObjectLayer>(layer)) {
        select_tool->set_object_layer(ol);
        place_tool->set_object_layer(ol);
        break;
      }
    }
    layer_selector->set_map(new_map);
    props_widget->clear();
    map_widget->update();
    window.statusBar()->showMessage(
      QStringLiteral("Sector: %1 (%2 tilemaps, %3 objects)")
        .arg(QString::fromStdString(last_level_meta->sector_name()))
        .arg(last_level_meta->tilemaps().size())
        .arg(last_level_meta->objects().size()));
  });

  // Status: always explain datadir / demo so the UI is not mysterious
  {
    QString msg;
    if (!supertux_datadir->empty()) {
      msg = QStringLiteral("datadir=%1").arg(QString::fromStdString(*supertux_datadir));
    } else {
      msg = QStringLiteral("datadir unset — placeholder tiles only");
    }
    if (options.demo && options.level.empty()) {
      msg += QStringLiteral(" | DEMO MAP (File→Open Level… or --level FILE)");
    }
    window.statusBar()->showMessage(msg);
  }

  if (!options.level.empty()) {
    if (options.verbose) {
      std::cerr << "[flexlay] auto-opening --level " << options.level << "\n";
    }
    // Trigger same path as menu by synthesizing open (call open_act)
    // Re-use logic: invoke open_act with path — simplest: QMetaObject or direct call
    // We queue a single-shot after show so widgets exist.
    QTimer::singleShot(0, &window, [&, path = QString::fromStdString(options.level)]() {
      // Duplicate open path via menu action is hard; call load inline by activating
      // File open is a lambda capturing many things — trigger via temporary set.
      // Prefer: programmatically click is not available; invoke stored open helper.
      std::cerr << "[flexlay] loading " << path.toStdString() << "…\n";
      // Fall back: use QFileDialog-free path by emitting through a local function pointer.
      // The open_act lambda uses QFileDialog — we need a shared loader. For now, use:
      try {
        auto level = flexlay::supertux::load_level_file(path.toStdString());
        std::shared_ptr<flexlay::Tileset> ts;
        flexlay::supertux::LoadedTilesetMeta meta;
        int image_tiles = 0;
        if (!supertux_datadir->empty()) {
          const std::string stts = *supertux_datadir + "/images/tiles.stts";
          flexlay::supertux::TilesetLoadOptions opts;
          opts.datadir = *supertux_datadir;
          opts.tile_size = tile_size;
          opts.load_images = true;
          try {
            auto loaded = flexlay::supertux::load_tileset_file(stts, opts);
            ts = loaded.first;
            meta = loaded.second;
            for (int id : ts->get_tiles()) {
              if (ts->create(id).has_image()) ++image_tiles;
            }
            std::cerr << "[flexlay] tileset: " << stts << " (" << image_tiles << " tiles with images)\n";
          } catch (const std::exception& tex) {
            std::cerr << "[flexlay] tileset load failed: " << tex.what() << "\n";
            QMessageBox::warning(&window, QStringLiteral("Tileset"), QString::fromUtf8(tex.what()));
          }
        } else {
          std::cerr << "[flexlay] no datadir — level geometry only, colored placeholder tiles\n";
        }
        if (!ts) ts = std::make_shared<flexlay::Tileset>(tile_size);
        for (const auto& tm : level.tilemaps()) {
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
        *last_level_meta = level;
        auto new_map = flexlay::supertux::editor_map_from_level(level, ts, *supertux_datadir);
        workspace->set_map(new_map);
        for (const auto& layer : new_map->layers()) {
          if (auto tml = std::dynamic_pointer_cast<flexlay::TilemapLayer>(layer)) {
            paint_tool->set_tilemap(tml);
            fill_tool->set_tilemap(tml);
            break;
          }
        }
        *current_tileset = ts;
        tile_selector->set_tileset(ts);
        layer_selector->set_map(new_map);
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
        sector_combo->blockSignals(true);
        sector_combo->clear();
        for (const auto& sn : level.sector_names()) {
          sector_combo->addItem(QString::fromStdString(sn));
        }
        sector_combo->setCurrentIndex(static_cast<int>(level.active_sector));
        sector_combo->setEnabled(level.sectors.size() > 1);
        sector_combo->blockSignals(false);
        map_widget->update();
        window.setWindowTitle(
          QStringLiteral("SuperTux Editor (C++) — %1")
            .arg(QString::fromStdString(level.name.empty() ? path.toStdString() : level.name)));
        window.statusBar()->showMessage(
          QStringLiteral("Loaded %1 [sector %2] (%3 tilemaps, %4 objects, %5 image tiles)")
            .arg(QString::fromStdString(level.name))
            .arg(QString::fromStdString(level.sector_name()))
            .arg(level.tilemaps().size())
            .arg(level.objects().size())
            .arg(image_tiles));
        std::cerr << "[flexlay] loaded OK: " << level.tilemaps().size() << " tilemaps, "
                  << level.objects().size() << " objects, " << image_tiles << " image tiles\n";
      } catch (const std::exception& ex) {
        std::cerr << "[flexlay] failed to open level: " << ex.what() << "\n";
        QMessageBox::warning(&window, QStringLiteral("Open Level"), QString::fromUtf8(ex.what()));
      }
    });
  }

  window.resize(1100, 640);
  window.show();
  map_widget->setFocus();
  return app.exec();
}
