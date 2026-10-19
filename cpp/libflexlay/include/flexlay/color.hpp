// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_COLOR_HPP
#define FLEXLAY_COLOR_HPP

#include <array>
#include <cstdio>
#include <string>

namespace flexlay {

/** 8-bit RGBA color. */
class Color
{
public:
  int r = 255;
  int g = 255;
  int b = 255;
  int a = 255;

  Color() = default;
  Color(int r_, int g_, int b_, int a_ = 255) : r(r_), g(g_), b(b_), a(a_) {}

  Color copy() const { return Color(r, g, b, a); }

  int get_red() const { return r; }
  int get_green() const { return g; }
  int get_blue() const { return b; }
  int get_alpha() const { return a; }

  bool operator==(const Color& rhs) const
  {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
  }
  bool operator!=(const Color& rhs) const { return !(*this == rhs); }

  std::array<int, 4> to_list() const { return {r, g, b, a}; }

  std::string to_hex() const
  {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
    return std::string(buf);
  }
};

/** Floating-point RGBA color in [0, 1]. */
class Colorf
{
public:
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float a = 1.0f;

  Colorf() = default;
  Colorf(float r_, float g_, float b_, float a_ = 1.0f) : r(r_), g(g_), b(b_), a(a_) {}

  std::array<float, 4> to_list() const { return {r, g, b, a}; }

  Color to_color() const
  {
    return Color(static_cast<int>(255 * r), static_cast<int>(255 * g),
                 static_cast<int>(255 * b), static_cast<int>(255 * a));
  }

  Color to_i() const { return to_color(); }

  bool operator==(const Colorf& other) const
  {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }
  bool operator!=(const Colorf& other) const { return !(*this == other); }
};

} // namespace flexlay

#endif
