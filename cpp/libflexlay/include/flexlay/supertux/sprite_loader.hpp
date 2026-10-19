// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_SPRITE_LOADER_HPP
#define FLEXLAY_SUPERTUX_SPRITE_LOADER_HPP

/**
 * Minimal SuperTux (supertux-sprite) loader for the editor.
 * Resolves the first usable action image (prefers default/left/editor)
 * and loads that PNG. Does not animate or mirror-action expand.
 */

#include "flexlay/sprite.hpp"

#include <optional>
#include <string>
#include <vector>

namespace flexlay {
namespace supertux {

struct SpriteAction
{
  std::string name;
  std::vector<std::string> images; // filenames relative to .sprite directory
  std::string mirror_action;       // if set, images come from another action
};

struct LoadedSpriteDef
{
  int version = 0;
  std::vector<SpriteAction> actions;
  /** Directory containing the .sprite file (for resolving relative images). */
  std::string base_dir;
};

/** Parse a .sprite file from disk. Throws on parse/I/O errors. */
LoadedSpriteDef load_sprite_def_file(const std::string& path);

/** Parse sexpr text; base_dir used only for returned structure. */
LoadedSpriteDef load_sprite_def_text(const std::string& text, const std::string& base_dir = {});

/**
 * Pick a preferred action's first image path (absolute), if any.
 * Preference order: action named "default", "editor", "left", "idle", "normal", then first with images.
 */
std::optional<std::string> preferred_image_path(const LoadedSpriteDef& def);

/**
 * Load Sprite from a .sprite path (absolute or relative to cwd).
 * Empty optional if no image resolves.
 */
std::optional<Sprite> load_sprite_from_def(const std::string& sprite_path);

} // namespace supertux
} // namespace flexlay

#endif
