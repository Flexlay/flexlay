// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_PROPERTY_BAG_HPP
#define FLEXLAY_PROPERTY_BAG_HPP

/**
 * Data-driven property bag for game objects (ECS-ish surface).
 * Values are typed scalars/strings; constraints/hooks stay outside.
 */

#include "flexlay/color.hpp"
#include "flexlay/math/point.hpp"

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace flexlay {

enum class PropertyType
{
  Bool,
  Int,
  Float,
  String,
  Color,
  Point,
};

using PropertyValue = std::variant<bool, int, float, std::string, Color, Pointf>;

inline PropertyType property_type_of(const PropertyValue& v)
{
  switch (v.index()) {
    case 0: return PropertyType::Bool;
    case 1: return PropertyType::Int;
    case 2: return PropertyType::Float;
    case 3: return PropertyType::String;
    case 4: return PropertyType::Color;
    case 5: return PropertyType::Point;
    default: return PropertyType::String;
  }
}

class PropertyBag
{
public:
  bool has(const std::string& key) const { return m_values.find(key) != m_values.end(); }

  void set(const std::string& key, PropertyValue value) { m_values[key] = std::move(value); }

  std::optional<PropertyValue> get(const std::string& key) const
  {
    const auto it = m_values.find(key);
    if (it == m_values.end()) {
      return std::nullopt;
    }
    return it->second;
  }

  template<typename T>
  std::optional<T> get_as(const std::string& key) const
  {
    const auto it = m_values.find(key);
    if (it == m_values.end()) {
      return std::nullopt;
    }
    if (const T* p = std::get_if<T>(&it->second)) {
      return *p;
    }
    return std::nullopt;
  }

  void erase(const std::string& key) { m_values.erase(key); }
  void clear() { m_values.clear(); }
  size_t size() const { return m_values.size(); }
  bool empty() const { return m_values.empty(); }

  std::vector<std::string> keys() const
  {
    std::vector<std::string> out;
    out.reserve(m_values.size());
    for (const auto& kv : m_values) {
      out.push_back(kv.first);
    }
    return out;
  }

  const std::map<std::string, PropertyValue>& values() const { return m_values; }
  std::map<std::string, PropertyValue>& values() { return m_values; }

private:
  std::map<std::string, PropertyValue> m_values;
};

} // namespace flexlay

#endif
