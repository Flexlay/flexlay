// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/field.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

namespace flexlay {

Field::Field(int width, int height, std::vector<uint32_t> data)
  : m_width(width)
  , m_height(height)
  , m_data(std::move(data))
{
  assert(m_width >= 0 && m_height >= 0);
  assert(static_cast<int>(m_data.size()) == m_width * m_height);
}

Field Field::from_size(int width, int height)
{
  if (width < 0 || height < 0) {
    throw std::invalid_argument("Field::from_size: negative dimension");
  }
  return Field(width, height, std::vector<uint32_t>(static_cast<size_t>(width * height), 0));
}

Field Field::from_list(int width, int height, const std::vector<uint32_t>& data)
{
  if (width < 0 || height < 0) {
    throw std::invalid_argument("Field::from_list: negative dimension");
  }
  if (static_cast<int>(data.size()) != width * height) {
    throw std::invalid_argument("Field::from_list: data size mismatch");
  }
  return Field(width, height, data);
}

Field Field::copy() const
{
  return Field(m_width, m_height, m_data);
}

Field Field::copy_region(int x, int y, int w, int h) const
{
  Field field = Field::from_size(w, h);

  const int start_x = std::max(0, -x);
  const int start_y = std::max(0, -y);
  const int end_x = std::min(m_width, w - x);
  const int end_y = std::min(m_height, h - y);

  for (int iy = start_y; iy < end_y; ++iy) {
    for (int ix = start_x; ix < end_x; ++ix) {
      field.put(x + ix, y + iy, at(ix, iy));
    }
  }
  return field;
}

void Field::put(int x, int y, uint32_t value)
{
  if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
    throw std::out_of_range("Field::put: coordinates out of range");
  }
  m_data[static_cast<size_t>(y * m_width + x)] = value;
}

uint32_t Field::at(int x, int y) const
{
  if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
    throw std::out_of_range("Field::at: coordinates out of range");
  }
  return m_data[static_cast<size_t>(y * m_width + x)];
}

void Field::resize(int w, int h, int x, int y)
{
  Field field = copy_region(x, y, w, h);
  m_width = field.m_width;
  m_height = field.m_height;
  m_data = std::move(field.m_data);
}

bool Field::contains(uint32_t key) const
{
  return std::find(m_data.begin(), m_data.end(), key) != m_data.end();
}

std::string Field::str() const
{
  std::ostringstream oss;
  oss << '\n';
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      oss << at(x, y) << ' ';
    }
    oss << '\n';
  }
  return oss.str();
}

} // namespace flexlay
