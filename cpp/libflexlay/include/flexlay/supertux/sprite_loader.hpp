// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_SPRITE_LOADER_HPP
#define FLEXLAY_SUPERTUX_SPRITE_LOADER_HPP

/**
 * Minimal SuperTux (supertux-sprite) loader for the editor.
 * Resolves a preferred action image and optional hitbox for placement guides.
 */

#include "flexlay/math/rect.hpp"
#include "flexlay/sprite.hpp"

#include <optional>
#include <string>
#include <vector>

namespace flexlay {
namespace supertux {

struct SpriteAction
{
  std::string name;
  std::vector<std::string> images; // relative to .sprite directory
  std::string mirror_action;
  /** Hitbox relative to sprite top-left: x, y, width, height. */
  float hitbox_x = 0;
  float hitbox_y = 0;
  float hitbox_w = 0;
  float hitbox_h = 0;
  bool has_hitbox = false;
  float fps = 15.0f;
};


struct LoadedSpriteDef
{
  int version = 0;
  std::vector<SpriteAction> actions;
  std::string base_dir;
};

LoadedSpriteDef load_sprite_def_file(const std::string& path);
LoadedSpriteDef load_sprite_def_text(const std::string& text, const std::string& base_dir = {});

/** Preferred action (default/editor/left/idle/normal/first with images). */
const SpriteAction* preferred_action(const LoadedSpriteDef& def);

std::optional<std::string> preferred_image_path(const LoadedSpriteDef& def);

/** All image paths (absolute) for preferred action, after mirror resolution. */
std::vector<std::string> preferred_image_paths(const LoadedSpriteDef& def);

/** Hitbox of preferred action in sprite-local coordinates, if present. */
std::optional<Rectf> preferred_hitbox(const LoadedSpriteDef& def);

std::optional<Sprite> load_sprite_from_def(const std::string& sprite_path);

} // namespace supertux
} // namespace flexlay

#endif
