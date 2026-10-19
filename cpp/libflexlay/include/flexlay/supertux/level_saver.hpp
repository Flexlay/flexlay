// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_LEVEL_SAVER_HPP
#define FLEXLAY_SUPERTUX_LEVEL_SAVER_HPP

/**
 * Write SuperTux-Origins-style level sexpr (.stlv).
 * Round-trips what the minimal loader understands (tilemaps + generic objects).
 */

#include "flexlay/editor_map.hpp"
#include "flexlay/supertux/level_loader.hpp"

#include <string>

namespace flexlay {
namespace supertux {

/** Serialize a LoadedLevel to sexpr text. */
std::string save_level_text(const LoadedLevel& level);

/** Write level sexpr to a file path. Throws on I/O error. */
void save_level_file(const LoadedLevel& level, const std::string& path);

/**
 * Rebuild a LoadedLevel from an EditorMap produced by editor_map_from_level
 * (or simpleed-compatible structure: TilemapLayers + ObjectLayer of rects).
 */
LoadedLevel level_from_editor_map(const EditorMap& map,
                                  const std::string& name = "unnamed",
                                  const std::string& sector_name = "main",
                                  int version = 4);

} // namespace supertux
} // namespace flexlay

#endif
