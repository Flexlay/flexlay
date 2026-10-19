// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_MATH_POINT_HPP
#define FLEXLAY_MATH_POINT_HPP

#include <string>

namespace flexlay {

class Pointf;

/** Integer 2D point. */
class Point
{
public:
  int x = 0;
  int y = 0;

  Point() = default;
  Point(int x_, int y_) : x(x_), y(y_) {}

  Pointf to_f() const;

  Point copy() const { return Point(x, y); }

  Point operator+(const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
  Point operator-(const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
  Point operator*(int rhs) const { return Point(x * rhs, y * rhs); }

  Point& operator+=(const Point& rhs) { x += rhs.x; y += rhs.y; return *this; }
  Point& operator-=(const Point& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
  Point& operator*=(int rhs) { x *= rhs; y *= rhs; return *this; }

  bool operator==(const Point& rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Point& rhs) const { return !(*this == rhs); }

  std::string str() const;
};

inline Point operator*(int lhs, const Point& rhs) { return rhs * lhs; }

/** Floating-point 2D point. */
class Pointf
{
public:
  float x = 0.0f;
  float y = 0.0f;

  Pointf() = default;
  Pointf(float x_, float y_) : x(x_), y(y_) {}

  Point to_i() const { return Point(static_cast<int>(x), static_cast<int>(y)); }

  Pointf copy() const { return Pointf(x, y); }

  Pointf operator+(const Pointf& rhs) const { return Pointf(x + rhs.x, y + rhs.y); }
  Pointf operator-(const Pointf& rhs) const { return Pointf(x - rhs.x, y - rhs.y); }
  Pointf operator*(float rhs) const { return Pointf(x * rhs, y * rhs); }

  Pointf& operator+=(const Pointf& rhs) { x += rhs.x; y += rhs.y; return *this; }
  Pointf& operator-=(const Pointf& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
  Pointf& operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }

  bool operator==(const Pointf& rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Pointf& rhs) const { return !(*this == rhs); }

  std::string str() const;
};

inline Pointf operator*(float lhs, const Pointf& rhs) { return rhs * lhs; }

inline Pointf Point::to_f() const
{
  return Pointf(static_cast<float>(x), static_cast<float>(y));
}

inline std::string Point::str() const
{
  return "Point(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

inline std::string Pointf::str() const
{
  return "Pointf(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

} // namespace flexlay

#endif
