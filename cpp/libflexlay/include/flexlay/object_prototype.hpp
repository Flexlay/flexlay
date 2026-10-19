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

#include <memory>
#include <string>
#include <vector>

namespace flexlay {

enum class ObjectShape
{
  Rect,
};

struct ObjectPrototype
{
  std::string type_id;
  std::string label;
  ObjectShape shape = ObjectShape::Rect;
  Sizef default_size{32.0f, 32.0f};
  Color default_color{200, 180, 80, 180};
  PropertyBag defaults;

  std::shared_ptr<ObjMapObject> instantiate(const Pointf& pos) const
  {
    auto obj = std::make_shared<ObjMapRectObject>(pos, default_size, default_color);
    obj->set_name(type_id);
    obj->properties() = defaults;
    obj->properties().set("type", type_id);
    return obj;
  }
};

/** Built-in demo / SuperTux-inspired prototypes (minimal set). */
inline std::vector<ObjectPrototype> default_object_prototypes()
{
  std::vector<ObjectPrototype> out;

  {
    ObjectPrototype p;
    p.type_id = "rect";
    p.label = "Rectangle";
    p.default_size = Sizef(64, 48);
    p.default_color = Color(255, 200, 50, 160);
    p.defaults.set("kind", std::string("marker"));
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "spawnpoint";
    p.label = "Spawn point";
    p.default_size = Sizef(32, 32);
    p.default_color = Color(80, 220, 120, 180);
    p.defaults.set("name", std::string("main"));
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "bonusblock";
    p.label = "Bonus block";
    p.default_size = Sizef(32, 32);
    p.default_color = Color(220, 140, 40, 200);
    p.defaults.set("data", 1);
    p.defaults.set("script", std::string(""));
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "candle";
    p.label = "Candle";
    p.default_size = Sizef(32, 64);
    p.default_color = Color(255, 220, 100, 200);
    p.defaults.set("burning", true);
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "ghosttree";
    p.label = "Ghost tree";
    p.default_size = Sizef(96, 128);
    p.default_color = Color(120, 160, 100, 180);
    out.push_back(std::move(p));
  }


  {
    ObjectPrototype p;
    p.type_id = "snowball";
    p.label = "Snowball";
    p.default_size = Sizef(32, 32);
    p.default_color = Color(220, 230, 255, 200);
    p.defaults.set("direction", std::string("auto"));
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "scripttrigger";
    p.label = "Script trigger";
    p.default_size = Sizef(64, 32);
    p.default_color = Color(180, 100, 255, 140);
    p.defaults.set("script", std::string(""));
    p.defaults.set("button", false);
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "powerup";
    p.label = "Powerup";
    p.default_size = Sizef(32, 32);
    p.default_color = Color(255, 80, 80, 200);
    p.defaults.set("sprite", std::string(""));
    out.push_back(std::move(p));
  }
  {
    ObjectPrototype p;
    p.type_id = "leveltime";
    p.label = "Level time";
    p.default_size = Sizef(32, 32);
    p.default_color = Color(100, 100, 255, 160);
    p.defaults.set("time", 300);
    out.push_back(std::move(p));
  }

  return out;
}

} // namespace flexlay

#endif
