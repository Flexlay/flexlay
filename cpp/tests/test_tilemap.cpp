// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/color.hpp"
#include "flexlay/editor_map.hpp"
#include "flexlay/graphic_context.hpp"
#include "flexlay/graphic_context_state.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/tilemap_layer.hpp"
#include "flexlay/tileset.hpp"
#include "flexlay/workspace.hpp"

#include <iostream>
#include <memory>
#include <vector>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

namespace {

class RecordingGC : public flexlay::GraphicContext
{
public:
  struct Fill { flexlay::Rectf rect; flexlay::Color color; };
  std::vector<Fill> fills;
  flexlay::Rectf clip{0, 0, 1000, 1000};

  void clear(const flexlay::Color&) override {}
  void draw_rect(const flexlay::Rectf&, const flexlay::Color&, int = 0) override {}
  void fill_rect(const flexlay::Rectf& rect, const flexlay::Color& color, int = 0) override
  {
    fills.push_back({rect, color});
  }
  void draw_line(float, float, float, float, const flexlay::Color&) override {}
  void push_modelview() override {}
  void pop_modelview() override {}
  void translate(float, float) override {}
  void scale(float, float) override {}
  void rotate(float) override {}
  flexlay::Rectf get_clip_rect() const override { return clip; }
  void set_clip_rect(const flexlay::Rectf& r) override { clip = r; }
};

} // namespace

int main()
{
  using namespace flexlay;

  auto tileset = std::make_shared<Tileset>(16);
  tileset->add_tile(1, Tile(Color(255, 0, 0)));
  tileset->add_tile(2, Tile(Color(0, 255, 0)));
  CHECK(tileset->get_tile_size() == 16);
  CHECK(tileset->has_tile(1));
  CHECK(!tileset->has_tile(99));

  auto layer = std::make_shared<TilemapLayer>(tileset, 4, 3);
  layer->field().put(1, 1, 1);
  layer->field().put(2, 1, 2);
  CHECK(layer->width() == 4);
  CHECK(layer->pixel_width() == 64);

  RecordingGC gc;
  layer->draw(gc);
  CHECK(gc.fills.size() == 2);

  auto map = std::make_shared<EditorMap>();
  map->set_bounding_rect(Rectf(0, 0, 64, 48));
  map->add_layer(layer);
  CHECK(map->layers().size() == 1);

  GraphicContextState state(200, 100);
  CHECK(state.get_zoom() == 1.0f);
  state.set_zoom(2.0f);
  CHECK(state.get_zoom() == 2.0f);

  Workspace ws;
  ws.set_map(map);
  ws.state().set_size(200, 100);
  RecordingGC gc2;
  ws.draw(gc2);
  CHECK(gc2.fills.size() >= 1);

  std::cout << "test_tilemap: ok\n";
  return 0;
}
