// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_MATH_SIZE_HPP
#define FLEXLAY_MATH_SIZE_HPP

#include <string>

namespace flexlay {

class Size
{
public:
  int width = 0;
  int height = 0;

  Size() = default;
  Size(int w, int h) : width(w), height(h) {}

  Size copy() const { return Size(width, height); }

  bool operator==(const Size& rhs) const { return width == rhs.width && height == rhs.height; }
  bool operator!=(const Size& rhs) const { return !(*this == rhs); }

  std::string str() const
  {
    return "Size(" + std::to_string(width) + ", " + std::to_string(height) + ")";
  }
};

class Sizef
{
public:
  float width = 0.0f;
  float height = 0.0f;

  Sizef() = default;
  Sizef(float w, float h) : width(w), height(h) {}

  Sizef copy() const { return Sizef(width, height); }
  Size to_i() const { return Size(static_cast<int>(width), static_cast<int>(height)); }

  bool operator==(const Sizef& rhs) const { return width == rhs.width && height == rhs.height; }
  bool operator!=(const Sizef& rhs) const { return !(*this == rhs); }

  std::string str() const
  {
    return "Sizef(" + std::to_string(width) + ", " + std::to_string(height) + ")";
  }
};

} // namespace flexlay

#endif
