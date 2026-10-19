// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/object_sprites.hpp"

#include "flexlay/color.hpp"
#include "flexlay/objmap_rect_object.hpp"
#include "flexlay/objmap_sprite_object.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/supertux/sprite_loader.hpp"

#include <unordered_map>

namespace flexlay {
namespace supertux {
namespace {

Color color_for_type(const std::string& type)
{
  unsigned h = 2166136261u;
  for (char c : type) {
    h ^= static_cast<unsigned char>(c);
    h *= 16777619u;
  }
  return Color(static_cast<int>(40 + (h % 180)),
               static_cast<int>(40 + ((h >> 8) % 180)),
               static_cast<int>(40 + ((h >> 16) % 180)), 180);
}

const std::unordered_map<std::string, ObjectVisualInfo>& visual_table()
{
  static const std::unordered_map<std::string, ObjectVisualInfo> table = {
    {"ambient_sound", {"engine/editor/ambientsound.png", Sizef(32, 32)}},
    {"angrystone", {"creatures/angrystone/angrystone.sprite", Sizef(32, 32)}},
    {"background", {"engine/editor/background.png", Sizef(32, 32)}},
    {"bicycle-platform", {"engine/editor/bicycleplatform.png", Sizef(32, 32)}},
    {"bonusblock", {"objects/bonus_block/bonusblock.sprite", Sizef(32, 32)}},
    {"bouncingsnowball", {"creatures/bouncing_snowball/left-0.png", Sizef(32, 32)}},
    {"camera", {"engine/editor/camera.png", Sizef(32, 32)}},
    {"candle", {"objects/candle/candle.sprite", Sizef(32, 32)}},
    {"captainsnowball", {"creatures/snowball/cpt-left-0.png", Sizef(32, 32)}},
    {"climbable", {"engine/editor/climbable.png", Sizef(32, 32)}},
    {"coin", {"objects/coin/coin.sprite", Sizef(32, 32)}},
    {"crystallo", {"creatures/crystallo/crystallo.sprite", Sizef(32, 32)}},
    {"darttrap", {"creatures/darttrap/darttrap.sprite", Sizef(32, 32)}},
    {"decal", {"engine/editor/decal.png", Sizef(32, 32)}},
    {"dispenser", {"creatures/dispenser/canon.png", Sizef(32, 32)}},
    {"door", {"objects/door/door.sprite", Sizef(32, 32)}},
    {"firefly", {"objects/resetpoints/default-resetpoint.sprite", Sizef(32, 32)}},
    {"fish", {"creatures/fish/left-0.png", Sizef(32, 32)}},
    {"flame", {"creatures/flame/flame.sprite", Sizef(32, 32)}},
    {"flying-platform", {"objects/flying_platform/flying_platform.sprite", Sizef(32, 32)}},
    {"flyingsnowball", {"creatures/flying_snowball/left-0.png", Sizef(32, 32)}},
    {"ghostflame", {"creatures/flame/ghostflame.sprite", Sizef(32, 32)}},
    {"ghosttree", {"creatures/flame/ghostflame.sprite", Sizef(32, 32)}},
    {"goldbomb", {"creatures/gold_bomb/gold_bomb.sprite", Sizef(32, 32)}},
    {"gradient", {"engine/editor/gradient.png", Sizef(32, 32)}},
    {"haywire", {"creatures/haywire/haywire.sprite", Sizef(32, 32)}},
    {"hurting_platform", {"objects/sawblade/sawblade.sprite", Sizef(32, 32)}},
    {"icecrusher", {"creatures/icecrusher/icecrusher.sprite", Sizef(32, 32)}},
    {"iceflame", {"creatures/flame/iceflame.sprite", Sizef(32, 32)}},
    {"igel", {"creatures/igel/igel.sprite", Sizef(32, 32)}},
    {"infoblock", {"objects/bonus_block/infoblock.sprite", Sizef(32, 32)}},
    {"invisible_wall", {"engine/editor/invisible_wall.png", Sizef(32, 32)}},
    {"ispy", {"objects/ispy/ispy.sprite", Sizef(32, 32)}},
    {"jumpy", {"creatures/snowjumpy/left-middle.png", Sizef(32, 32)}},
    {"kugelblitz", {"creatures/kugelblitz/flying-0.png", Sizef(32, 32)}},
    {"lantern", {"objects/lantern/lantern.sprite", Sizef(32, 32)}},
    {"leveltime", {"engine/editor/clock.png", Sizef(32, 32)}},
    {"livefire", {"creatures/livefire/livefire.sprite", Sizef(32, 32)}},
    {"livefire_asleep", {"creatures/livefire/livefire.sprite", Sizef(32, 32)}},
    {"livefire_dormant", {"creatures/livefire/livefire.sprite", Sizef(32, 32)}},
    {"magicblock", {"objects/magicblock/magicblock.sprite", Sizef(32, 32)}},
    {"mole", {"creatures/mole/mole.sprite", Sizef(32, 32)}},
    {"mrbomb", {"creatures/mr_bomb/left.png", Sizef(32, 32)}},
    {"mriceblock", {"creatures/mr_iceblock/left-0.png", Sizef(32, 32)}},
    {"mrtree", {"creatures/mr_tree/walk-left-1.png", Sizef(32, 32)}},
    {"owl", {"creatures/owl/owl.sprite", Sizef(32, 32)}},
    {"particles-ghosts", {"engine/editor/ghostparticles.png", Sizef(32, 32)}},
    {"platform", {"objects/flying_platform/flying_platform-0.png", Sizef(32, 32)}},
    {"pneumatic-platform", {"engine/editor/pneumaticplatform.png", Sizef(32, 32)}},
    {"poisonivy", {"creatures/poison_ivy/left-0.png", Sizef(32, 32)}},
    {"powerup", {"engine/editor/powerup.png", Sizef(32, 32)}},
    {"pushbutton", {"objects/pushbutton/pushbutton.sprite", Sizef(32, 32)}},
    {"rustytrampoline", {"objects/rusty-trampoline/rusty-trampoline.sprite", Sizef(32, 32)}},
    {"scriptedobject", {"engine/editor/scriptedobject.png", Sizef(32, 32)}},
    {"scripttrigger", {"engine/editor/scripttrigger.png", Sizef(32, 32)}},
    {"secretarea", {"engine/editor/secretarea.png", Sizef(32, 32)}},
    {"sequencetrigger", {"engine/editor/sequencetrigger.png", Sizef(32, 32)}},
    {"short_fuse", {"creatures/short_fuse/short_fuse.sprite", Sizef(32, 32)}},
    {"skullyhop", {"creatures/skullyhop/skullyhop.sprite", Sizef(32, 32)}},
    {"smartball", {"creatures/snowball/left-1.png", Sizef(32, 32)}},
    {"smartblock", {"creatures/mr_iceblock/smart_block/smart_block.sprite", Sizef(32, 32)}},
    {"snail", {"creatures/snail/snail.sprite", Sizef(32, 32)}},
    {"snowball", {"creatures/snowball/sport-left-1.png", Sizef(32, 32)}},
    {"snowman", {"creatures/snowman/snowman.sprite", Sizef(32, 32)}},
    {"spawnpoint", {"engine/editor/spawnpoint.png", Sizef(32, 32)}},
    {"spidermite", {"creatures/spidermite/spidermite.sprite", Sizef(32, 32)}},
    {"spiky", {"creatures/spiky/left-0.png", Sizef(32, 32)}},
    {"spotlight", {"objects/spotlight/spotlight_center.sprite", Sizef(32, 32)}},
    {"sspiky", {"creatures/spiky/sleeping-left.png", Sizef(32, 32)}},
    {"stalactite", {"creatures/stalactite/falling.png", Sizef(32, 32)}},
    {"stalactite_yeti", {"engine/editor/stalactite_yeti.png", Sizef(32, 32)}},
    {"stumpy", {"creatures/mr_tree/stumpy.sprite", Sizef(32, 32)}},
    {"switch", {"objects/switch/switch.sprite", Sizef(32, 32)}},
    {"thunderstorm", {"engine/editor/thunderstorm.png", Sizef(32, 32)}},
    {"toad", {"creatures/toad/toad.sprite", Sizef(32, 32)}},
    {"torch", {"objects/torch/torch1.sprite", Sizef(32, 32)}},
    {"totem", {"creatures/totem/totem.sprite", Sizef(32, 32)}},
    {"trampoline", {"objects/trampoline/trampoline.sprite", Sizef(32, 32)}},
    {"tux", {"creatures/tux/big/jump-0.png", Sizef(32, 32)}},
    {"walkingleaf", {"creatures/walkingleaf/walkingleaf.sprite", Sizef(32, 32)}},
    {"walkingtree", {"creatures/walkingleaf/walkingleaf.sprite", Sizef(32, 32)}},
    {"weak_block", {"objects/weak_block/meltbox.sprite", Sizef(32, 32)}},
    {"willowisp", {"creatures/willowisp/willowisp.sprite", Sizef(32, 32)}},
    {"wind", {"engine/editor/wind.png", Sizef(32, 32)}},
    {"yeti", {"creatures/yeti/yeti.png", Sizef(32, 32)}},
    {"yeti_stalactite", {"engine/editor/stalactite_yeti.png", Sizef(32, 32)}},
    {"zeekling", {"creatures/zeekling/left-0.png", Sizef(32, 32)}},
    {"level", {"worldmap/common/leveldot.sprite", Sizef(32, 32)}},
    {"worldmap-spawnpoint", {"worldmap/common/tux.sprite", Sizef(32, 32)}},
    {"special-tile", {"engine/editor/secretarea.png", Sizef(32, 32)}},
    {"sprite-change", {"worldmap/common/leveldot.sprite", Sizef(32, 32)}},
    {"teleporter", {"worldmap/common/teleporterdown.sprite", Sizef(32, 32)}},
  };

return table;
}

} // namespace

std::optional<ObjectVisualInfo> object_visual_info(const std::string& type)
{
  const auto& table = visual_table();
  const auto it = table.find(type);
  if (it == table.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::optional<Sprite> try_load_sprite(const std::string& datadir, const std::string& rel_path)
{
  if (datadir.empty() || rel_path.empty()) {
    return std::nullopt;
  }
  // Paths in levels are often "/images/..." or "images/..."
  std::string rel = rel_path;
  if (rel.size() >= 8 && rel.compare(0, 8, "/images/") == 0) {
    rel = rel.substr(8);
  } else if (rel.size() >= 7 && rel.compare(0, 7, "images/") == 0) {
    rel = rel.substr(7);
  }
  const std::string path = datadir + "/images/" + rel;

  // .sprite definition: resolve first frame PNG via sprite_loader
  if (rel.size() > 7 && rel.compare(rel.size() - 7, 7, ".sprite") == 0) {
    if (auto spr = load_sprite_from_def(path)) {
      return spr;
    }
    return std::nullopt;
  }

  try {
    return Sprite(PixelBuffer::from_file(path), rel);
  } catch (...) {
    return std::nullopt;
  }
}

std::shared_ptr<ObjMapObject> make_editor_object(const std::string& type,
                                                const Pointf& pos,
                                                float width,
                                                float height,
                                                const PropertyBag& props,
                                                const std::string& datadir)
{
  std::string image_rel;
  if (auto sp = props.get_as<std::string>("sprite")) {
    image_rel = *sp;
    // .sprite files are not loadable as PNG — try common PNG sibling if needed
    // .sprite paths are resolved by try_load_sprite
  }
  Sizef size(width > 0 ? width : 32.0f, height > 0 ? height : 32.0f);
  if (auto info = object_visual_info(type)) {
    if (image_rel.empty()) {
      image_rel = info->editor_image;
    }
    if (width <= 0) {
      size.width = info->default_size.width;
    }
    if (height <= 0) {
      size.height = info->default_size.height;
    }
  }

  // .sprite: load all frames for editor animation
  if (!datadir.empty() && !image_rel.empty()) {
    std::string rel = image_rel;
    if (rel.size() >= 8 && rel.compare(0, 8, "/images/") == 0) rel = rel.substr(8);
    else if (rel.size() >= 7 && rel.compare(0, 7, "images/") == 0) rel = rel.substr(7);
    const std::string path = datadir + "/images/" + rel;
    if (rel.size() > 7 && rel.compare(rel.size() - 7, 7, ".sprite") == 0) {
      try {
        const auto def = load_sprite_def_file(path);
        const auto paths = preferred_image_paths(def);
        std::vector<Sprite> frames;
        for (const auto& ip : paths) {
          try {
            frames.push_back(Sprite(PixelBuffer::from_file(ip), ip));
          } catch (...) {
          }
        }
        if (!frames.empty()) {
          auto obj = std::make_shared<ObjMapSpriteObject>(frames.front(), pos);
          float fps = 15.0f;
          if (const SpriteAction* act = preferred_action(def)) {
            fps = act->fps;
          }
          if (frames.size() > 1) {
            obj->set_frames(std::move(frames), fps);
          }
          obj->set_name(type);
          obj->properties() = props;
          obj->properties().set("type", type);
          obj->properties().set("editor-image", image_rel);
          return obj;
        }
      } catch (...) {
      }
    }
  }

  if (auto spr = try_load_sprite(datadir, image_rel)) {
    auto obj = std::make_shared<ObjMapSpriteObject>(*spr, pos);
    obj->set_name(type);
    obj->properties() = props;
    obj->properties().set("type", type);
    if (!image_rel.empty()) {
      obj->properties().set("editor-image", image_rel);
    }
    return obj;
  }

  auto obj = std::make_shared<ObjMapRectObject>(pos, size, color_for_type(type));
  obj->set_name(type);
  obj->properties() = props;
  obj->properties().set("type", type);
  return obj;
}


std::shared_ptr<ObjMapObject> clone_editor_object(const ObjMapObject& src,
                                                  const Pointf& offset,
                                                  const std::string& datadir)
{
  std::string type = src.name();
  if (auto t = src.properties().get_as<std::string>("type")) {
    type = *t;
  }
  float w = 32;
  float h = 32;
  if (const auto* rect = dynamic_cast<const ObjMapRectObject*>(&src)) {
    w = rect->get_size().width;
    h = rect->get_size().height;
  }
  Pointf pos = src.get_pos();
  pos.x += offset.x;
  pos.y += offset.y;
  auto obj = make_editor_object(type, pos, w, h, src.properties(), datadir);
  obj->set_name(src.name());
  return obj;
}

} // namespace supertux
} // namespace flexlay