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
    {"spawnpoint", {
      {"name", "Name", std::string("main")},
    }},
    {"leveltime", {
      {"time", "Time", 300},
    }},
    {"camera", {
      {"mode", "Mode", std::string("normal")},
      {"backscrolling", "Backscrolling", true},
    }},
    {"bonusblock", {
      {"data", "Contents", 1},
      {"script", "Script", std::string("")},
    }},
    {"infoblock", {
      {"message", "Message", std::string("")},
    }},
    {"powerup", {
      {"sprite", "Sprite", std::string("")},
    }},
    {"candle", {
      {"burning", "Burning", true},
    }},
    {"torch", {
      {"lit", "Lit", true},
    }},
    {"scripttrigger", {
      {"script", "Script", std::string("")},
      {"button", "Button", false},
    }},
    {"sequencetrigger", {
      {"sequence", "Sequence", std::string("")},
    }},
    {"door", {
      {"sector", "Sector", std::string("")},
      {"spawnpoint", "Spawnpoint", std::string("")},
    }},
    {"trampoline", {
      {"portable", "Portable", true},
    }},
    {"flame", {
      {"radius", "Radius", 100.0f},
      {"speed", "Speed", 2.0f},
    }},
    {"ghostflame", {
      {"radius", "Radius", 100.0f},
      {"speed", "Speed", 2.0f},
    }},
    {"decal", {
      {"sprite", "Sprite", std::string("")},
    }},
    {"wind", {
      {"blowing", "Blowing", true},
      {"speed", "Speed", 1.0f},
    }},
    {"pushbutton", {
      {"script", "Script", std::string("")},
    }},
    {"switch", {
      {"script", "Script", std::string("")},
    }},
    {"platform", {
      {"name", "Name", std::string("")},
    }},
    {"secretarea", {
      {"script", "Script", std::string("")},
      {"message", "Message", std::string("")},
    }},
    {"ambient_sound", {
      {"file", "Sample", std::string("")},
      {"distance_factor", "Distance factor", 0.1f},
      {"volume", "Volume", 1.0f},
    }},
    {"dispenser", {
      {"badguy", "Badguy", std::string("snowball")},
      {"cycle", "Cycle", 5.0f},
    }},
    {"snowball", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"jumpy", {
      {"direction", "Direction", std::string("left")},
    }},
    {"mrbomb", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"mriceblock", {
      {"direction", "Direction", std::string("auto")},
    }},
    {"weak_block", {
      {"linked", "Linked", false},
    }},
    {"coin", {}},
    {"firefly", {
      {"sprite", "Sprite", std::string("")},
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
