// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_MATH_RECT_HPP
#define FLEXLAY_MATH_RECT_HPP

#include "point.hpp"
#include "size.hpp"

#include <string>
#include <utility>

namespace flexlay {

class Rectf;

/** Axis-aligned rectangle with integer edges (left, top, right, bottom). */
class Rect
{
public:
  int left = 0;
  int top = 0;
  int right = 0;
  int bottom = 0;

  Rect() = default;
  Rect(int left_, int top_, int right_, int bottom_)
    : left(left_), top(top_), right(right_), bottom(bottom_)
  {}

  static Rect from_ps(const Point& point, const Size& size)
  {
    return Rect(point.x, point.y, point.x + size.width, point.y + size.height);
  }

  static Rect zero() { return Rect(0, 0, 0, 0); }

  Rect copy() const { return Rect(left, top, right, bottom); }

  int width() const { return right - left; }
  int height() const { return bottom - top; }
  Size size() const { return Size(width(), height()); }

  bool is_inside(const Point& point) const
  {
    return left <= point.x && point.x <= right && top <= point.y && point.y <= bottom;
  }

  bool is_overlapped(const Rect& rect) const
  {
    return rect.left < right && rect.right > left && rect.top < bottom && rect.bottom > top;
  }

  void set_size(const Size& s)
  {
    right = left + s.width;
    bottom = top + s.height;
  }

  void normalize()
  {
    if (left > right) {
      std::swap(left, right);
    }
    if (top > bottom) {
      std::swap(top, bottom);
    }
  }

  Rectf to_f() const;

  bool operator==(const Rect& rhs) const
  {
    return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
  }
  bool operator!=(const Rect& rhs) const { return !(*this == rhs); }

  std::string str() const;
};

/** Axis-aligned rectangle with float edges. */
class Rectf
{
public:
  float left = 0.0f;
  float top = 0.0f;
  float right = 0.0f;
  float bottom = 0.0f;

  Rectf() = default;
  Rectf(float left_, float top_, float right_, float bottom_)
    : left(left_), top(top_), right(right_), bottom(bottom_)
  {}

  static Rectf from_ps(const Pointf& point, const Sizef& size)
  {
    return Rectf(point.x, point.y, point.x + size.width, point.y + size.height);
  }

  static Rectf zero() { return Rectf(0, 0, 0, 0); }

  Rectf copy() const { return Rectf(left, top, right, bottom); }

  float width() const { return right - left; }
  float height() const { return bottom - top; }
  Sizef size() const { return Sizef(width(), height()); }

  bool is_inside(const Pointf& point) const
  {
    return left <= point.x && point.x <= right && top <= point.y && point.y <= bottom;
  }

  bool is_overlapped(const Rectf& rect) const
  {
    return rect.left < right && rect.right > left && rect.top < bottom && rect.bottom > top;
  }

  void set_size(const Sizef& s)
  {
    right = left + s.width;
    bottom = top + s.height;
  }

  void normalize()
  {
    if (left > right) {
      std::swap(left, right);
    }
    if (top > bottom) {
      std::swap(top, bottom);
    }
  }

  /** Convert using edge values (left, top, right, bottom). */
  Rect to_i() const
  {
    return Rect(static_cast<int>(left), static_cast<int>(top),
                static_cast<int>(right), static_cast<int>(bottom));
  }

  bool operator==(const Rectf& rhs) const
  {
    return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
  }
  bool operator!=(const Rectf& rhs) const { return !(*this == rhs); }

  std::string str() const;
};

inline Rectf Rect::to_f() const
{
  return Rectf(static_cast<float>(left), static_cast<float>(top),
               static_cast<float>(right), static_cast<float>(bottom));
}

inline std::string Rect::str() const
{
  return "Rect(" + std::to_string(left) + ", " + std::to_string(top) + ", " +
         std::to_string(right) + ", " + std::to_string(bottom) + ")";
}

inline std::string Rectf::str() const
{
  return "Rectf(" + std::to_string(left) + ", " + std::to_string(top) + ", " +
         std::to_string(right) + ", " + std::to_string(bottom) + ")";
}

} // namespace flexlay

#endif
