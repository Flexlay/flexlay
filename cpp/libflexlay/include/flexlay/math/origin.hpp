// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_MATH_ORIGIN_HPP
#define FLEXLAY_MATH_ORIGIN_HPP

#include "point.hpp"
#include "size.hpp"

#include <stdexcept>

namespace flexlay {

enum class Origin
{
  top_left = 0,
  top_center = 1,
  top_right = 2,
  center_left = 3,
  center = 4,
  center_right = 5,
  bottom_left = 6,
  bottom_center = 7,
  bottom_right = 8,
};

inline Pointf calc_originf(Origin origin, const Sizef& size)
{
  switch (origin) {
    case Origin::top_left:
      return Pointf(0, 0);
    case Origin::top_center:
      return Pointf(size.width / 2.0f, 0);
    case Origin::top_right:
      return Pointf(size.width, 0);
    case Origin::center_left:
      return Pointf(0, size.height / 2.0f);
    case Origin::center:
      return Pointf(size.width / 2.0f, size.height / 2.0f);
    case Origin::center_right:
      return Pointf(size.width, size.height / 2.0f);
    case Origin::bottom_left:
      return Pointf(0, size.height);
    case Origin::bottom_center:
      return Pointf(size.width / 2.0f, size.height);
    case Origin::bottom_right:
      return Pointf(size.width, size.height);
  }
  throw std::invalid_argument("invalid origin value");
}

inline Point calc_origin(Origin origin, const Size& size)
{
  switch (origin) {
    case Origin::top_left:
      return Point(0, 0);
    case Origin::top_center:
      return Point(size.width / 2, 0);
    case Origin::top_right:
      return Point(size.width, 0);
    case Origin::center_left:
      return Point(0, size.height / 2);
    case Origin::center:
      return Point(size.width / 2, size.height / 2);
    case Origin::center_right:
      return Point(size.width, size.height / 2);
    case Origin::bottom_left:
      return Point(0, size.height);
    case Origin::bottom_center:
      return Point(size.width / 2, size.height);
    case Origin::bottom_right:
      return Point(size.width, size.height);
  }
  throw std::invalid_argument("invalid origin value");
}

} // namespace flexlay

#endif
