// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_OBJECT_SPRITES_HPP
#define FLEXLAY_SUPERTUX_OBJECT_SPRITES_HPP

/**
 * Default editor images and sizes for common SuperTux object types.
 * Paths are relative to $datadir/images/.
 */

#include "flexlay/math/size.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/objmap_object.hpp"
#include "flexlay/property_bag.hpp"
#include "flexlay/sprite.hpp"

#include <memory>
#include <optional>
#include <string>

namespace flexlay {
namespace supertux {

struct ObjectVisualInfo
{
  std::string editor_image; // relative to images/
  Sizef default_size{32, 32};
};

/** Lookup known type defaults; nullopt if unknown. */
std::optional<ObjectVisualInfo> object_visual_info(const std::string& type);

/**
 * Try load sprite from datadir/images/rel_path. Empty datadir or failure → empty optional.
 */
std::optional<Sprite> try_load_sprite(const std::string& datadir, const std::string& rel_path);

/**
 * Build editor object: ObjMapSpriteObject if a sprite loads, else colored rect.
 * Uses props["sprite"] path if present, else object_visual_info(type).
 */
std::shared_ptr<ObjMapObject> make_editor_object(const std::string& type,
                                                const Pointf& pos,
                                                float width,
                                                float height,
                                                const PropertyBag& props,
                                                const std::string& datadir);

/** Clone an editor object (type + props + optional size); offset applied to position. */
std::shared_ptr<ObjMapObject> clone_editor_object(const ObjMapObject& src,
                                                 const Pointf& offset,
                                                 const std::string& datadir = {});

} // namespace supertux
} // namespace flexlay

#endif
