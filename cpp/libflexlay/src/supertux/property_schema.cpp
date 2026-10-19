// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/supertux/property_schema.hpp"

#include <unordered_map>
#include <unordered_set>

namespace flexlay {
namespace supertux {
namespace {

using Schema = std::vector<PropertyField>;

const Schema& empty_schema()
{
  static const Schema s;
  return s;
}

const std::unordered_map<std::string, Schema>& all_schemas()
{
    static const std::unordered_map<std::string, Schema> map = {
    {"ambient_sound", {
      {"distance_factor", "Distance Factor", 0.0f},
      {"distance_bias", "Distance Bias", 0.0f},
      {"volume", "Max Volume", 0},
    }},
    {"background", {
      {"x", "X", 0.0f},
      {"y", "Y", 0.0f},
      {"alignment", "Alignment", std::string("")},
      {"speed", "Speed (X)", 0.0f},
      {"speed_y", "Speed (Y)", 0.0f},
    }},
    {"bonusblock", {
      {"data", "Contents", 1},
      {"script", "Script", std::string("")},
      {"message", "Message", std::string("")},
      {"count", "Count", 1},
      {"sprite", "Sprite", std::string("")},
    }},
    {"camera", {
      {"mode", "Mode", std::string("")},
      {"backscrolling", "Backscrollling", true},
    }},
    {"candle", {
      {"name", "Name", std::string("")},
      {"burning", "Burning", true},
      {"flicker", "Flicker", true},
      {"sprite", "Sprite", std::string("")},
    }},
    {"darttrap", {
      {"initial-delay", "Initial Delay", 0.0f},
      {"fire-delay", "Fire Delay", 2.0f},
      {"ammo", "Ammo", -1},
      {"direction", "Direction", std::string("auto")},
    }},
    {"decal", {
      {"sprite", "Sprite", std::string("")},
    }},
    {"dispenser", {
      {"cycle", "Cycle", 0},
      {"type", "Type", std::string("")},
      {"random", "Random", false},
      {"direction", "Direction", std::string("auto")},
    }},
    {"door", {
      {"sector", "Sector", std::string("")},
      {"spawnpoint", "SpawnPoint", std::string("")},
    }},
    {"firefly", {
      {"sprite", "Sprite", std::string("")},
    }},
    {"flame", {
      {"radius", "Radius", 100.0f},
      {"speed", "Speed", 2.0f},
    }},
    {"ghostflame", {
      {"radius", "Radius", 100.0f},
      {"speed", "Speed", 2.0f},
    }},
    {"infoblock", {
      {"message", "Message", std::string("")},
    }},
    {"jumpy", {
      {"direction", "Direction", std::string("left")},
    }},
    {"leveltime", {
      {"time", "Time", 300},
    }},
    {"mrbomb", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"mriceblock", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"platform", {
      {"name", "Name", std::string("")},
      {"running", "Running", true},
      {"sprite", "Sprite", std::string("")},
    }},
    {"powerup", {
      {"disable-physics", "Disable Physics", false},
      {"script", "Script", std::string("")},
      {"sprite", "Sprite", std::string("")},
    }},
    {"pushbutton", {
      {"script", "Script", std::string("")},
      {"sprite", "Sprite", std::string("")},
      {"direction", "Direction", std::string("auto")},
    }},
    {"scriptedobject", {
      {"name", "Name", std::string("")},
      {"visible", "Visible", false},
      {"physic-enabled", "Physics", false},
      {"solid", "Solid", false},
      {"sprite", "Sprite", std::string("")},
    }},
    {"scripttrigger", {
      {"script", "Script", std::string("")},
      {"button", "Button", false},
    }},
    {"secretarea", {
      {"script", "Script", std::string("")},
      {"message", "Message", std::string("")},
    }},
    {"sequencetrigger", {
      {"sequence", "Sequence", std::string("")},
    }},
    {"snowball", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"spawnpoint", {
      {"name", "Name", std::string("main")},
    }},
    {"switch", {
      {"script", "Script", std::string("")},
      {"sprite", "Sprite", std::string("")},
      {"direction", "Direction", std::string("auto")},
    }},
    {"torch", {
      {"sprite", "Sprite", std::string("")},
    }},
    {"trampoline", {
      {"portable", "Portable", true},
    }},
    {"weak_block", {
      {"linked", "Linked", false},
    }},
    {"willowisp", {
      {"spawnpoint", "SpawnPoint", std::string("")},
      {"flyspeed", "Fly Speed", 64.0f},
      {"track-range", "Track Range", 384.0f},
      {"vanish-range", "Vanish Range", 512.0f},
      {"hit-script", "Hit Script", std::string("")},
    }},
    {"wind", {
      {"name", "Name", std::string("")},
      {"blowing", "Blowing", true},
      {"speed-x", "Speed-X", 0.0f},
      {"speed-y", "Speed-Y", 0.0f},
      {"acceleration", "Acceleration", 0.0f},
    }},
  };
return map;
}

bool is_internal_key(const std::string& key)
{
  return key == "kind" || key == "type";
}

} // namespace

const std::vector<PropertyField>& property_schema_for(const std::string& type_id)
{
  const auto& map = all_schemas();
  auto it = map.find(type_id);
  if (it == map.end()) {
    return empty_schema();
  }
  return it->second;
}

void ensure_property_schema(PropertyBag& bag, const std::string& type_id)
{
  for (const auto& field : property_schema_for(type_id)) {
    if (!bag.has(field.key)) {
      bag.set(field.key, field.default_value);
    }
  }
}

std::string property_label(const std::string& type_id, const std::string& key)
{
  for (const auto& field : property_schema_for(type_id)) {
    if (field.key == key) {
      return field.label;
    }
  }
  return key;
}

std::vector<std::string> property_display_keys(const PropertyBag& bag, const std::string& type_id)
{
  std::vector<std::string> out;
  std::unordered_set<std::string> seen;
  for (const auto& field : property_schema_for(type_id)) {
    if (bag.has(field.key)) {
      out.push_back(field.key);
      seen.insert(field.key);
    }
  }
  for (const auto& key : bag.keys()) {
    if (seen.count(key) || is_internal_key(key)) {
      continue;
    }
    out.push_back(key);
  }
  return out;
}

} // namespace supertux
} // namespace flexlay
