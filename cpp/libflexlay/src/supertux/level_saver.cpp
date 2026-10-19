// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/level_saver.hpp"

#include "flexlay/object_layer.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/tilemap_layer.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace flexlay {
namespace supertux {
namespace {

void write_indent(std::ostream& out, int n)
{
  for (int i = 0; i < n; ++i) {
    out << "  ";
  }
}

std::string escape_string(const std::string& s)
{
  std::string out;
  out.reserve(s.size() + 2);
  for (char c : s) {
    if (c == '\\' || c == '"') {
      out.push_back('\\');
    }
    out.push_back(c);
  }
  return out;
}

void write_atom(std::ostream& out, const PropertyValue& v)
{
  if (const auto* b = std::get_if<bool>(&v)) {
    out << (*b ? "#t" : "#f");
  } else if (const auto* i = std::get_if<int>(&v)) {
    out << *i;
  } else if (const auto* f = std::get_if<float>(&v)) {
    out << *f;
  } else if (const auto* s = std::get_if<std::string>(&v)) {
    out << '"' << escape_string(*s) << '"';
  } else if (const auto* c = std::get_if<Color>(&v)) {
    out << c->r << ' ' << c->g << ' ' << c->b << ' ' << c->a;
  } else if (const auto* p = std::get_if<Pointf>(&v)) {
    out << p->x << ' ' << p->y;
  } else {
    out << "\"\"";
  }
}

std::string strip_solid_suffix(std::string name)
{
  const std::string suffix = " [solid]";
  if (name.size() >= suffix.size() &&
      name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
    name.erase(name.size() - suffix.size());
  }
  return name;
}

void write_sector(std::ostream& out, const LoadedSector& sector)
{
  write_indent(out, 2);
  out << "(sector\n";
  write_indent(out, 3);
  out << "(name \"" << escape_string(sector.name.empty() ? "main" : sector.name) << "\")\n";
  write_indent(out, 3);
  out << "(objects\n";

  if (sector.has_gravity) {
    write_indent(out, 4);
    out << "(gravity " << sector.gravity << ")\n";
  }
  if (sector.has_ambient_light) {
    write_indent(out, 4);
    out << "(ambient-light " << sector.ambient_r << " " << sector.ambient_g << " "
        << sector.ambient_b << " " << sector.ambient_a << ")\n";
  }

  for (const auto& tm : sector.tilemaps) {
    write_indent(out, 4);
    out << "(tilemap\n";
    if (!tm.name.empty()) {
      write_indent(out, 5);
      out << "(name \"" << escape_string(strip_solid_suffix(tm.name)) << "\")\n";
    }
    write_indent(out, 5);
    out << "(solid " << (tm.solid ? "#t" : "#f") << ")\n";
    write_indent(out, 5);
    out << "(z-pos " << tm.z_pos << ")\n";
    write_indent(out, 5);
    out << "(width " << tm.width << ")\n";
    write_indent(out, 5);
    out << "(height " << tm.height << ")\n";
    write_indent(out, 5);
    out << "(tiles";
    for (size_t i = 0; i < tm.tiles.size(); ++i) {
      if (i % 16 == 0) {
        out << "\n";
        write_indent(out, 6);
      } else {
        out << ' ';
      }
      out << tm.tiles[i];
    }
    out << ")\n";
    write_indent(out, 4);
    out << ")\n";
  }

  for (const auto& obj : sector.objects) {
    write_indent(out, 4);
    out << '(' << (obj.type.empty() ? "unknown" : obj.type) << '\n';
    write_indent(out, 5);
    out << "(x " << obj.x << ")\n";
    write_indent(out, 5);
    out << "(y " << obj.y << ")\n";
    if (obj.width > 0 && std::fabs(obj.width - 32.0f) > 0.01f) {
      write_indent(out, 5);
      out << "(width " << obj.width << ")\n";
    }
    if (obj.height > 0 && std::fabs(obj.height - 32.0f) > 0.01f) {
      write_indent(out, 5);
      out << "(height " << obj.height << ")\n";
    }
    for (const auto& kv : obj.props.values()) {
      if (kv.first == "type") {
        continue;
      }
      write_indent(out, 5);
      out << '(' << kv.first << ' ';
      write_atom(out, kv.second);
      out << ")\n";
    }
    write_indent(out, 4);
    out << ")\n";
  }

  write_indent(out, 3);
  out << ")\n"; // objects
  write_indent(out, 2);
  out << ")\n"; // sector
}

} // namespace

std::string save_level_text(const LoadedLevel& level)
{
  std::ostringstream out;
  out << "(supertux-level\n";
  write_indent(out, 1);
  out << "(version " << (level.version > 0 ? level.version : 4) << ")\n";
  write_indent(out, 1);
  out << "(name \"" << escape_string(level.name.empty() ? "unnamed" : level.name) << "\")\n";
  if (!level.author.empty()) {
    write_indent(out, 1);
    out << "(author \"" << escape_string(level.author) << "\")\n";
  }
  if (!level.license.empty()) {
    write_indent(out, 1);
    out << "(license \"" << escape_string(level.license) << "\")\n";
  }
  if (!level.contact.empty()) {
    write_indent(out, 1);
    out << "(contact \"" << escape_string(level.contact) << "\")\n";
  }
  if (!level.tileset_path.empty()) {
    write_indent(out, 1);
    out << "(tileset \"" << escape_string(level.tileset_path) << "\")\n";
  }
  if (level.target_time > 0) {
    write_indent(out, 1);
    out << "(target-time " << level.target_time << ")\n";
  }
  write_indent(out, 1);
  out << "(sectors\n";

  if (level.sectors.empty()) {
    // Synthesize a single empty main sector so the file stays valid
    LoadedSector empty;
    empty.name = "main";
    write_sector(out, empty);
  } else {
    for (const auto& sector : level.sectors) {
      write_sector(out, sector);
    }
  }

  write_indent(out, 1);
  out << ")\n"; // sectors
  out << ")\n"; // supertux-level
  return out.str();
}

void save_level_file(const LoadedLevel& level, const std::string& path)
{
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("save_level_file: cannot open " + path);
  }
  out << save_level_text(level);
  if (!out) {
    throw std::runtime_error("save_level_file: write failed for " + path);
  }
}

LoadedLevel level_from_editor_map(const EditorMap& map, const std::string& name,
                                  const std::string& sector_name, int version,
                                  const LoadedLevel* meta)
{
  LoadedLevel level;
  level.name = name;
  level.version = version;

  // Start from meta sectors so inactive sectors are preserved
  if (meta) {
    if (level.name == "unnamed" && !meta->name.empty()) {
      level.name = meta->name;
    }
    if (level.version <= 0 && meta->version > 0) {
      level.version = meta->version;
    }
    level.author = meta->author;
    level.license = meta->license;
    level.contact = meta->contact;
    level.tileset_path = meta->tileset_path;
    level.target_time = meta->target_time;
    level.is_worldmap = meta->is_worldmap;
    level.sectors = meta->sectors;
    level.active_sector = meta->active_sector;
  }

  if (level.sectors.empty()) {
    LoadedSector s;
    s.name = sector_name.empty() ? "main" : sector_name;
    level.sectors.push_back(std::move(s));
    level.active_sector = 0;
  } else if (!sector_name.empty()) {
    level.select_sector(sector_name);
  }

  LoadedSector& sector = level.sectors[level.active_sector];
  if (!sector_name.empty()) {
    sector.name = sector_name;
  }

  sector.tilemaps.clear();
  sector.objects.clear();

  int z = 0;
  for (const auto& layer : map.layers()) {
    if (auto* tm = dynamic_cast<TilemapLayer*>(layer.get())) {
      LoadedTilemap lt;
      lt.name = tm->get_name();
      lt.solid = lt.name.find("[solid]") != std::string::npos;
      lt.z_pos = z++;
      lt.width = tm->width();
      lt.height = tm->height();
      lt.tiles.resize(static_cast<size_t>(lt.width * lt.height), 0);
      for (int y = 0; y < lt.height; ++y) {
        for (int x = 0; x < lt.width; ++x) {
          lt.tiles[static_cast<size_t>(y * lt.width + x)] = tm->field().at(x, y);
        }
      }
      sector.tilemaps.push_back(std::move(lt));
      continue;
    }
    if (auto* ol = dynamic_cast<ObjectLayer*>(layer.get())) {
      for (const auto& obj : ol->objects()) {
        LoadedObject lo;
        lo.type = obj->name();
        if (auto type = obj->properties().get_as<std::string>("type")) {
          lo.type = *type;
        }
        // Worldmap stores tile coordinates in the file
        const float inv = level.is_worldmap ? (1.0f / 32.0f) : 1.0f;
        lo.x = obj->get_pos().x * inv;
        lo.y = obj->get_pos().y * inv;
        lo.width = 32;
        lo.height = 32;
        if (auto* rect = dynamic_cast<ObjMapRectObject*>(obj.get())) {
          lo.width = rect->get_size().width;
          lo.height = rect->get_size().height;
        }
        lo.props = obj->properties();
        sector.objects.push_back(std::move(lo));
      }
    }
  }
  return level;
}

} // namespace supertux
} // namespace flexlay
