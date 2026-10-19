// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJECT_PROTOTYPE_HPP
#define FLEXLAY_OBJECT_PROTOTYPE_HPP

/**
 * Data-driven object type descriptors for the editor palette.
 * Not a port of SuperTux gameobjs.py — a small, extensible registry.
 */

#include "flexlay/color.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/size.hpp"
#include "flexlay/objmap_object.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/property_bag.hpp"
#include "flexlay/supertux/object_sprites.hpp"

#include <memory>
#include <string>
#include <vector>

namespace flexlay {

struct ObjectPrototype
{
  std::string type_id;
  std::string label;
  Sizef default_size{32.0f, 32.0f};
  Color default_color{200, 180, 80, 180};
  std::string editor_image; // relative to images/; optional
  PropertyBag defaults;

  std::shared_ptr<ObjMapObject> instantiate(const Pointf& pos,
                                            const std::string& datadir = {}) const
  {
    PropertyBag props = defaults;
    props.set("type", type_id);
    if (!editor_image.empty()) {
      props.set("editor-image", editor_image);
    }
    auto obj = supertux::make_editor_object(type_id, pos, default_size.width, default_size.height,
                                            props, datadir);
    obj->set_name(type_id);
    return obj;
  }
};

/** Built-in demo / SuperTux-inspired prototypes (minimal set). */
inline std::vector<ObjectPrototype> default_object_prototypes()
{
  std::vector<ObjectPrototype> out;

  auto add = [&](const char* id, const char* label, Sizef size, Color color,
                 const char* image, auto fill_props) {
    ObjectPrototype p;
    p.type_id = id;
    p.label = label;
    p.default_size = size;
    p.default_color = color;
    if (image && *image) {
      p.editor_image = image;
    }
    // Prefer SuperTux visual table sizes when known
    if (auto info = supertux::object_visual_info(id)) {
      if (p.editor_image.empty()) {
        p.editor_image = info->editor_image;
      }
      p.default_size = info->default_size;
    }
    fill_props(p.defaults);
    out.push_back(std::move(p));
  };

  add("rect", "Rectangle", Sizef(64, 48), Color(255, 200, 50, 160), "",
      [](PropertyBag& d) { d.set("kind", std::string("marker")); });
  add("spawnpoint", "Spawn point", Sizef(32, 32), Color(80, 220, 120, 180), "",
      [](PropertyBag& d) { d.set("name", std::string("main")); });
  add("bonusblock", "Bonus block", Sizef(32, 32), Color(220, 140, 40, 200), "",
      [](PropertyBag& d) {
        d.set("data", 1);
        d.set("script", std::string(""));
      });
  add("candle", "Candle", Sizef(32, 64), Color(255, 220, 100, 200), "",
      [](PropertyBag& d) { d.set("burning", true); });
  add("ghosttree", "Ghost tree", Sizef(96, 128), Color(120, 160, 100, 180), "",
      [](PropertyBag&) {});
  add("snowball", "Snowball", Sizef(32, 32), Color(220, 230, 255, 200), "",
      [](PropertyBag& d) { d.set("direction", std::string("auto")); });
  add("scripttrigger", "Script trigger", Sizef(64, 32), Color(180, 100, 255, 140), "",
      [](PropertyBag& d) {
        d.set("script", std::string(""));
        d.set("button", false);
      });
  add("powerup", "Powerup", Sizef(32, 32), Color(255, 80, 80, 200), "",
      [](PropertyBag& d) { d.set("sprite", std::string("")); });
  add("leveltime", "Level time", Sizef(32, 32), Color(100, 100, 255, 160), "",
      [](PropertyBag& d) { d.set("time", 300); });
  add("mr_tree", "Mr. Tree", Sizef(64, 64), Color(100, 160, 80, 180), "",
      [](PropertyBag&) {});
  add("coin", "Coin", Sizef(32, 32), Color(255, 220, 50, 200), "",
      [](PropertyBag&) {});
  add("heavycoin", "Heavy coin", Sizef(32, 32), Color(255, 200, 40, 200), "",
      [](PropertyBag&) {});
  add("rock", "Rock", Sizef(32, 32), Color(140, 120, 100, 200), "",
      [](PropertyBag&) {});
  add("jumpy", "Jumpy", Sizef(32, 32), Color(200, 100, 200, 200), "",
      [](PropertyBag& d) { d.set("direction", std::string("left")); });
  add("trampoline", "Trampoline", Sizef(32, 32), Color(80, 200, 120, 200), "",
      [](PropertyBag&) {});
  add("invisible_wall", "Invisible wall", Sizef(32, 32), Color(255, 0, 0, 80), "",
      [](PropertyBag&) {});
  add("infoblock", "Info block", Sizef(32, 32), Color(100, 180, 255, 200), "",
      [](PropertyBag& d) { d.set("message", std::string("")); });

  return out;
}

} // namespace flexlay

#endif
