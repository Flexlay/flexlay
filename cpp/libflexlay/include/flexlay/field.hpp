// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_FIELD_HPP
#define FLEXLAY_FIELD_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace flexlay {

/**
 * 2D grid of uint32 tile indices.
 * Row-major storage: index = y * width + x (matches former numpy [y][x]).
 */
class Field
{
public:
  Field() = default;

  static Field from_size(int width, int height);
  static Field from_list(int width, int height, const std::vector<uint32_t>& data);

  Field copy() const;
  Field copy_region(int x, int y, int w, int h) const;

  void put(int x, int y, uint32_t value);
  uint32_t at(int x, int y) const;

  /** Resize so this field becomes a w×h region taken from offset (x, y). */
  void resize(int w, int h, int x = 0, int y = 0);

  int width() const { return m_width; }
  int height() const { return m_height; }
  int size() const { return m_width * m_height; }

  bool contains(uint32_t key) const;

  std::string str() const;

  const std::vector<uint32_t>& data() const { return m_data; }

private:
  Field(int width, int height, std::vector<uint32_t> data);

  int m_width = 0;
  int m_height = 0;
  std::vector<uint32_t> m_data;
};

} // namespace flexlay

#endif
