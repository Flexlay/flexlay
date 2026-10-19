// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/tileset_loader.hpp"

#include "flexlay/pixel_buffer.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/util/sexpr.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace flexlay {
namespace supertux {
namespace {

const SExpr* find_child(const SExpr& list, const std::string& tag)
{
  if (!list.is_list()) {
    return nullptr;
  }
  for (const auto& child : list.as_list()) {
    if (child.is_list() && child.tag() == tag) {
      return &child;
    }
  }
  return nullptr;
}

std::string child_atom(const SExpr& list, const std::string& tag, const std::string& def = {})
{
  const SExpr* c = find_child(list, tag);
  if (!c || c->as_list().size() < 2 || !c->as_list()[1].is_atom()) {
    return def;
  }
  return c->as_list()[1].as_atom();
}

int child_int(const SExpr& list, const std::string& tag, int def = 0)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  try {
    return std::stoi(a);
  } catch (...) {
    return def;
  }
}

bool child_bool(const SExpr& list, const std::string& tag, bool def = false)
{
  const std::string a = child_atom(list, tag);
  if (a.empty()) {
    return def;
  }
  return a == "#t" || a == "true" || a == "1";
}

/** Collect string atoms under (images ...) or (image ...). */
std::vector<std::string> read_image_list(const SExpr& node)
{
  std::vector<std::string> out;
  for (const char* key : {"images", "image", "editor-images"}) {
    const SExpr* imgs = find_child(node, key);
    if (!imgs || !imgs->is_list()) {
      continue;
    }
    for (size_t i = 1; i < imgs->as_list().size(); ++i) {
      const auto& e = imgs->as_list()[i];
      if (e.is_atom()) {
        out.push_back(e.as_atom());
      }
    }
    if (!out.empty()) {
      break;
    }
  }
  return out;
}

Color placeholder_color(int id)
{
  // Distinct-ish palette from id
  const uint8_t r = static_cast<uint8_t>(40 + (id * 37) % 180);
  const uint8_t g = static_cast<uint8_t>(40 + (id * 73) % 180);
  const uint8_t b = static_cast<uint8_t>(40 + (id * 19) % 180);
  return Color(r, g, b, 255);
}

void add_tile_def(Tileset& tileset, int id, const std::string& image_rel,
                  const TilesetLoadOptions& opts, LoadedTilesetMeta& meta)
{
  if (id == 0) {
    return;
  }
  Tile tile(placeholder_color(id));
  if (!image_rel.empty()) {
    tile.set_filename(image_rel);
    if (opts.load_images && !opts.datadir.empty()) {
      const std::string path = opts.datadir + "/images/" + image_rel;
      try {
        tile = Tile(PixelBuffer::from_file(path));
        tile.set_filename(image_rel);
      } catch (...) {
        // keep placeholder
      }
    }
  }
  tileset.add_tile(id, tile);
  ++meta.tile_count;
}

void parse_single_tile(const SExpr& node, Tileset& tileset, const TilesetLoadOptions& opts,
                       LoadedTilesetMeta& meta)
{
  const int id = child_int(node, "id");
  if (id == 0) {
    return;
  }
  const auto images = read_image_list(node);
  const std::string img = images.empty() ? std::string{} : images.front();
  add_tile_def(tileset, id, img, opts, meta);
  (void)child_bool(node, "solid"); // attributes reserved for later
}

void parse_bulk_tiles(const SExpr& node, Tileset& tileset, const TilesetLoadOptions& opts,
                      LoadedTilesetMeta& meta)
{
  // (tiles (ids 1 2 3 ...) (width 4) (image "sheet.png") (attributes ...) ...)
  const SExpr* ids_node = find_child(node, "ids");
  if (!ids_node || !ids_node->is_list()) {
    return;
  }
  std::vector<int> ids;
  for (size_t i = 1; i < ids_node->as_list().size(); ++i) {
    const auto& a = ids_node->as_list()[i];
    if (!a.is_atom()) {
      continue;
    }
    try {
      ids.push_back(std::stoi(a.as_atom()));
    } catch (...) {
      ids.push_back(0);
    }
  }

  std::string sheet;
  for (const char* key : {"image", "images"}) {
    const SExpr* img = find_child(node, key);
    if (img && img->is_list() && img->as_list().size() >= 2 && img->as_list()[1].is_atom()) {
      sheet = img->as_list()[1].as_atom();
      break;
    }
  }

  for (int id : ids) {
    add_tile_def(tileset, id, sheet, opts, meta);
  }
}

void parse_tilegroup(const SExpr& node, LoadedTilesetMeta& meta)
{
  TileGroupInfo g;
  g.name = child_atom(node, "name");
  const SExpr* tiles = find_child(node, "tiles");
  if (tiles && tiles->is_list()) {
    for (size_t i = 1; i < tiles->as_list().size(); ++i) {
      const auto& a = tiles->as_list()[i];
      if (!a.is_atom()) {
        continue;
      }
      try {
        g.tiles.push_back(std::stoi(a.as_atom()));
      } catch (...) {
      }
    }
  }
  meta.tilegroups.push_back(std::move(g));
}

void walk(const SExpr& node, Tileset& tileset, const TilesetLoadOptions& opts, LoadedTilesetMeta& meta)
{
  if (!node.is_list() || node.as_list().empty()) {
    return;
  }
  const std::string tag = node.tag();
  if (tag == "tile") {
    parse_single_tile(node, tileset, opts, meta);
    return;
  }
  if (tag == "tiles") {
    // bulk sheet mapping
    if (find_child(node, "ids")) {
      parse_bulk_tiles(node, tileset, opts, meta);
      return;
    }
    // container of individual (tile ...) entries
    for (size_t i = 1; i < node.as_list().size(); ++i) {
      if (node.as_list()[i].is_list()) {
        walk(node.as_list()[i], tileset, opts, meta);
      }
    }
    return;
  }
  if (tag == "tilegroup") {
    parse_tilegroup(node, meta);
    return;
  }
  if (tag == "tilegroups" || tag == "supertux-tileset" || tag == "autotilesets") {
    for (size_t i = 1; i < node.as_list().size(); ++i) {
      walk(node.as_list()[i], tileset, opts, meta);
    }
    return;
  }
  // generic walk
  for (size_t i = 1; i < node.as_list().size(); ++i) {
    if (node.as_list()[i].is_list()) {
      walk(node.as_list()[i], tileset, opts, meta);
    }
  }
}

} // namespace

std::pair<std::shared_ptr<Tileset>, LoadedTilesetMeta>
load_tileset_text(const std::string& text, const TilesetLoadOptions& opts)
{
  const SExpr root = sexpr_parse(text);
  if (root.tag() != "supertux-tileset" && root.tag() != "supertux-tiles") {
    throw std::runtime_error("load_tileset: root is not supertux-tileset/supertux-tiles");
  }

  auto tileset = std::make_shared<Tileset>(opts.tile_size);
  LoadedTilesetMeta meta;
  walk(root, *tileset, opts, meta);
  return {tileset, meta};
}

std::pair<std::shared_ptr<Tileset>, LoadedTilesetMeta>
load_tileset_file(const std::string& path, const TilesetLoadOptions& opts)
{
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("load_tileset_file: cannot open " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  auto result = load_tileset_text(ss.str(), opts);
  result.second.filename = path;
  return result;
}

} // namespace supertux
} // namespace flexlay
