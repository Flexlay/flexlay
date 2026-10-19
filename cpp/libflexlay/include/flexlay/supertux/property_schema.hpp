// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_SUPERTUX_PROPERTY_SCHEMA_HPP
#define FLEXLAY_SUPERTUX_PROPERTY_SCHEMA_HPP

/**
 * Known SuperTux object property keys (labels + defaults) for the editor form.
 * Not a full port of gameobjs.py — ensures common fields appear when selected.
 */

#include "flexlay/property_bag.hpp"

#include <string>
#include <vector>

namespace flexlay {
namespace supertux {

struct PropertyField
{
  std::string key;
  std::string label;
  PropertyValue default_value;
};

/** Schema for a type_id; empty if unknown. */
const std::vector<PropertyField>& property_schema_for(const std::string& type_id);

/**
 * Ensure schema keys exist on bag (insert defaults when missing).
 * Does not overwrite existing values.
 */
void ensure_property_schema(PropertyBag& bag, const std::string& type_id);

/** Prefer schema label for key; else return key. */
std::string property_label(const std::string& type_id, const std::string& key);

/**
 * Ordered keys for the properties form: schema order first, then any extra keys.
 * Skips internal keys (kind).
 */
std::vector<std::string> property_display_keys(const PropertyBag& bag, const std::string& type_id);

} // namespace supertux
} // namespace flexlay

#endif
