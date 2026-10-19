// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_OBJECT_PROTOTYPE_HPP
#define FLEXLAY_OBJECT_PROTOTYPE_HPP

/**
 * Data-driven object type descriptors for the editor palette.
 * Data-driven registry generated from SuperTux editor badguys/gameobjs lists
 * (images only; no Python embedding).
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
    if (auto info = supertux::object_visual_info(id)) {
      if (p.editor_image.empty()) {
        p.editor_image = info->editor_image;
      }
      p.default_size = info->default_size;
    }
    fill_props(p.defaults);
    out.push_back(std::move(p));
  };

  // Marker rect for editor-only annotations
  add("rect", "Rectangle", Sizef(64, 48), Color(255, 200, 50, 160), "",
      [](PropertyBag& d) { d.set("kind", std::string("marker")); });

  add("ambient_sound", "Ambient sound", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("angrystone", "Angrystone", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("background", "Background", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("bicycle-platform", "Bicycle platform", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("bonusblock", "Bonusblock", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("data", 1);
        d.set("script", std::string(""));
      });
  add("bouncingsnowball", "Bouncingsnowball", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("camera", "Camera", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("candle", "Candle", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("burning", true);
      });
  add("captainsnowball", "Captainsnowball", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("climbable", "Climbable", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("coin", "Coin", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("crystallo", "Crystallo", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("darttrap", "Darttrap", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("decal", "Decal", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("dispenser", "Dispenser", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("door", "Door", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("firefly", "Firefly", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("fish", "Fish", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("flame", "Flame", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("flying-platform", "Flying platform", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("flyingsnowball", "Flyingsnowball", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("ghostflame", "Ghostflame", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("ghosttree", "Ghosttree", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("goldbomb", "Goldbomb", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("gradient", "Gradient", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("haywire", "Haywire", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("hurting_platform", "Hurting platform", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("icecrusher", "Icecrusher", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("iceflame", "Iceflame", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("igel", "Igel", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("infoblock", "Infoblock", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("message", std::string(""));
      });
  add("invisible_wall", "Invisible wall", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("ispy", "Ispy", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("jumpy", "Jumpy", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("direction", std::string("left"));
      });
  add("kugelblitz", "Kugelblitz", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("lantern", "Lantern", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("leveltime", "Leveltime", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("time", 300);
      });
  add("livefire", "Livefire", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("livefire_asleep", "Livefire asleep", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("livefire_dormant", "Livefire dormant", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("magicblock", "Magicblock", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("mole", "Mole", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("mrbomb", "Mrbomb", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("mriceblock", "Mriceblock", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("mrtree", "Mrtree", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("owl", "Owl", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("particles-ghosts", "Particles ghosts", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("platform", "Platform", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("pneumatic-platform", "Pneumatic platform", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("poisonivy", "Poisonivy", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("powerup", "Powerup", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("sprite", std::string(""));
      });
  add("pushbutton", "Pushbutton", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("rustytrampoline", "Rustytrampoline", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("scriptedobject", "Scriptedobject", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("scripttrigger", "Scripttrigger", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("script", std::string(""));
        d.set("button", false);
      });
  add("secretarea", "Secretarea", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("sequencetrigger", "Sequencetrigger", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("short_fuse", "Short fuse", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("skullyhop", "Skullyhop", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("smartball", "Smartball", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("smartblock", "Smartblock", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("snail", "Snail", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("snowball", "Snowball", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("direction", std::string("auto"));
      });
  add("snowman", "Snowman", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("spawnpoint", "Spawnpoint", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag& d) {
        d.set("name", std::string("main"));
      });
  add("spidermite", "Spidermite", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("spiky", "Spiky", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("spotlight", "Spotlight", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("sspiky", "Sspiky", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("stalactite", "Stalactite", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("stalactite_yeti", "Stalactite yeti", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("stumpy", "Stumpy", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("switch", "Switch", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("thunderstorm", "Thunderstorm", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("toad", "Toad", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("torch", "Torch", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("totem", "Totem", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("trampoline", "Trampoline", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("tux", "Tux", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("walkingleaf", "Walkingleaf", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("walkingtree", "Walkingtree", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("weak_block", "Weak block", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("willowisp", "Willowisp", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("wind", "Wind", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("yeti", "Yeti", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("yeti_stalactite", "Yeti stalactite", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});
  add("zeekling", "Zeekling", Sizef(32, 32), Color(180, 120, 120, 200), "",
      [](PropertyBag&) {});

  add("level", "Level (worldmap)", Sizef(32, 32), Color(80, 200, 80, 200), "",
      [](PropertyBag& d) { d.set("level", std::string("")); });
  add("worldmap-spawnpoint", "Worldmap spawn", Sizef(32, 32), Color(80, 120, 220, 200), "",
      [](PropertyBag& d) { d.set("name", std::string("main")); });
  add("special-tile", "Special tile", Sizef(32, 32), Color(200, 80, 200, 160), "",
      [](PropertyBag&) {});
  add("sprite-change", "Sprite change", Sizef(32, 32), Color(180, 180, 80, 200), "",
      [](PropertyBag&) {});
  add("teleporter", "Teleporter", Sizef(32, 32), Color(100, 100, 200, 200), "",
      [](PropertyBag&) {});
  return out;
}

} // namespace flexlay

#endif
