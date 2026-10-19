// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/tileset_loader.hpp"

#include "flexlay/pixel_buffer.hpp"
#include "flexlay/tile.hpp"
#include "flexlay/util/sexpr.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <map>
#include <sstream>
#include <vector>
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

/** Image reference: either a path or a region crop inside a sheet. */
struct ImageRef
{
  std::string path;
  bool is_region = false;
  int rx = 0;
  int ry = 0;
  int rw = 0;
  int rh = 0;
};

/**
 * Collect image refs from (images ...), (image ...), or (editor-images ...).
 * Prefers editor-images when present (editor display), else images/image.
 * Supports atoms and (region "file" x y w h).
 */
std::vector<ImageRef> read_image_refs(const SExpr& node, bool prefer_editor)
{
  std::vector<ImageRef> out;
  const char* keys_editor[] = {"editor-images", "images", "image"};
  const char* keys_game[] = {"images", "image", "editor-images"};
  const char** keys = prefer_editor ? keys_editor : keys_game;
  const int nkeys = 3;

  for (int k = 0; k < nkeys; ++k) {
    const SExpr* imgs = find_child(node, keys[k]);
    if (!imgs || !imgs->is_list()) {
      continue;
    }
    for (size_t i = 1; i < imgs->as_list().size(); ++i) {
      const auto& e = imgs->as_list()[i];
      if (e.is_atom()) {
        ImageRef r;
        r.path = e.as_atom();
        out.push_back(std::move(r));
      } else if (e.is_list() && e.tag() == "region" && e.as_list().size() >= 6) {
        // (region "file" x y w h)
        ImageRef r;
        if (!e.as_list()[1].is_atom()) {
          continue;
        }
        r.path = e.as_list()[1].as_atom();
        r.is_region = true;
        try {
          r.rx = std::stoi(e.as_list()[2].as_atom());
          r.ry = std::stoi(e.as_list()[3].as_atom());
          r.rw = std::stoi(e.as_list()[4].as_atom());
          r.rh = std::stoi(e.as_list()[5].as_atom());
        } catch (...) {
          continue;
        }
        out.push_back(std::move(r));
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
  const uint8_t r = static_cast<uint8_t>(40 + (id * 37) % 180);
  const uint8_t g = static_cast<uint8_t>(40 + (id * 73) % 180);
  const uint8_t b = static_cast<uint8_t>(40 + (id * 19) % 180);
  return Color(r, g, b, 255);
}

/** Simple path → shared PixelBuffer cache for one load_tileset call. */
class ImageCache
{
public:
  explicit ImageCache(std::string datadir)
    : m_datadir(std::move(datadir))
  {
  }

  std::shared_ptr<PixelBuffer> get(const std::string& rel_path)
  {
    if (rel_path.empty() || m_datadir.empty()) {
      return nullptr;
    }
    const auto it = m_cache.find(rel_path);
    if (it != m_cache.end()) {
      return it->second;
    }
    const std::string full = m_datadir + "/images/" + rel_path;
    try {
      auto pb = std::make_shared<PixelBuffer>(PixelBuffer::from_file(full));
      m_cache[rel_path] = pb;
      return pb;
    } catch (...) {
      m_cache[rel_path] = nullptr;
      return nullptr;
    }
  }

private:
  std::string m_datadir;
  std::map<std::string, std::shared_ptr<PixelBuffer>> m_cache;
};

Tile make_tile_from_ref(const ImageRef& ref, ImageCache* cache, int id)
{
  Tile tile(placeholder_color(id));
  if (ref.path.empty()) {
    return tile;
  }
  tile.set_filename(ref.path);

  if (!cache) {
    return tile;
  }

  auto sheet = cache->get(ref.path);
  if (!sheet || sheet->empty()) {
    return tile;
  }

  if (ref.is_region) {
    PixelBuffer cropped = sheet->subregion(ref.rx, ref.ry, ref.rw, ref.rh);
    if (!cropped.empty()) {
      tile = Tile(std::move(cropped));
      tile.set_filename(ref.path);
    }
  } else {
    tile = Tile(*sheet);
    tile.set_filename(ref.path);
  }
  return tile;
}

void add_tile_def(Tileset& tileset, int id, const ImageRef& ref, ImageCache* cache,
                  LoadedTilesetMeta& meta)
{
  if (id == 0) {
    return;
  }
  tileset.add_tile(id, make_tile_from_ref(ref, cache, id));
  ++meta.tile_count;
}

void parse_single_tile(const SExpr& node, Tileset& tileset, ImageCache* cache,
                       LoadedTilesetMeta& meta)
{
  const int id = child_int(node, "id");
  if (id == 0) {
    return;
  }
  // Prefer editor-images for the editor view (matches Python SuperTuxTileset).
  const auto refs = read_image_refs(node, /*prefer_editor=*/true);
  ImageRef ref;
  if (!refs.empty()) {
    ref = refs.front();
  }
  add_tile_def(tileset, id, ref, cache, meta);
  (void)child_bool(node, "solid"); // attributes reserved for later
}

void parse_bulk_tiles(const SExpr& node, Tileset& tileset, const TilesetLoadOptions& opts,
                      ImageCache* cache, LoadedTilesetMeta& meta)
{
  // (tiles (ids 1 2 3 ...) (width 4) (image "sheet.png") ...)
  // Sheet is sliced into tile_size×tile_size cells, row-major left-to-right,
  // top-to-bottom; width is columns per row.
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

  const int width = child_int(node, "width", 1);
  const int cols = width > 0 ? width : 1;
  const int tile_px = opts.tile_size > 0 ? opts.tile_size : 32;

  std::shared_ptr<PixelBuffer> sheet_pb;
  if (cache && !sheet.empty()) {
    sheet_pb = cache->get(sheet);
  }

  int x = 0;
  int y = 0;
  for (int id : ids) {
    if (id != 0) {
      ImageRef ref;
      ref.path = sheet;
      if (sheet_pb && !sheet_pb->empty()) {
        ref.is_region = true;
        ref.rx = x * tile_px;
        ref.ry = y * tile_px;
        ref.rw = tile_px;
        ref.rh = tile_px;
        PixelBuffer cropped = sheet_pb->subregion(ref.rx, ref.ry, ref.rw, ref.rh);
        Tile tile = cropped.empty() ? Tile(placeholder_color(id)) : Tile(std::move(cropped));
        tile.set_filename(sheet);
        tileset.add_tile(id, tile);
        ++meta.tile_count;
      } else {
        add_tile_def(tileset, id, ref, cache, meta);
      }
    }
    x += 1;
    if (x >= cols) {
      x = 0;
      y += 1;
    }
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

void walk(const SExpr& node, Tileset& tileset, const TilesetLoadOptions& opts, ImageCache* cache,
          LoadedTilesetMeta& meta)
{
  if (!node.is_list() || node.as_list().empty()) {
    return;
  }
  const std::string tag = node.tag();
  if (tag == "tile") {
    parse_single_tile(node, tileset, cache, meta);
    return;
  }
  if (tag == "tiles") {
    // bulk sheet mapping
    if (find_child(node, "ids")) {
      parse_bulk_tiles(node, tileset, opts, cache, meta);
      return;
    }
    // container of individual (tile ...) entries
    for (size_t i = 1; i < node.as_list().size(); ++i) {
      if (node.as_list()[i].is_list()) {
        walk(node.as_list()[i], tileset, opts, cache, meta);
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
      walk(node.as_list()[i], tileset, opts, cache, meta);
    }
    return;
  }
  // generic walk
  for (size_t i = 1; i < node.as_list().size(); ++i) {
    if (node.as_list()[i].is_list()) {
      walk(node.as_list()[i], tileset, opts, cache, meta);
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

  std::unique_ptr<ImageCache> cache;
  if (opts.load_images && !opts.datadir.empty()) {
    cache = std::make_unique<ImageCache>(opts.datadir);
  }

  walk(root, *tileset, opts, cache.get(), meta);
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


std::string resolve_tileset_path(const std::string& datadir,
                                 const std::string& tileset_path_from_level)
{
  namespace fs = std::filesystem;
  const std::string fallback = datadir.empty() ? std::string("images/tiles.stts")
                                               : (datadir + "/images/tiles.stts");
  if (datadir.empty()) {
    return tileset_path_from_level.empty() ? fallback : tileset_path_from_level;
  }
  auto try_path = [](const fs::path& p) -> std::string {
    if (fs::is_regular_file(p)) {
      return p.string();
    }
    return {};
  };
  std::string rel = tileset_path_from_level;
  // normalize
  while (!rel.empty() && rel[0] == '/') {
    rel = rel.substr(1);
  }
  if (rel.size() >= 7 && rel.compare(0, 7, "images/") == 0) {
    // keep as-is under datadir
  }
  const std::vector<fs::path> candidates = {
    fs::path(datadir) / rel,
    fs::path(datadir) / "images" / fs::path(rel).filename(),
    fs::path(datadir) / rel.substr(rel.find("images/") == 0 ? 7 : 0),
  };
  for (const auto& c : candidates) {
    if (auto s = try_path(c); !s.empty()) {
      return s;
    }
  }
  // extension swap .strf <-> try common defaults
  if (!rel.empty()) {
    fs::path p = fs::path(datadir) / "images" / fs::path(rel).filename();
    if (auto s = try_path(p); !s.empty()) {
      return s;
    }
  }
  return fallback;
}

} // namespace supertux
} // namespace flexlay
