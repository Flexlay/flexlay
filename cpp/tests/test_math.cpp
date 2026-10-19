// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/math/origin.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/math/size.hpp"
#include "flexlay/color.hpp"

#include <cstdlib>
#include <iostream>

#define CHECK(cond)                                                                            \
  do {                                                                                         \
    if (!(cond)) {                                                                             \
      std::cerr << "CHECK failed: " #cond " at " << __FILE__ << ":" << __LINE__ << std::endl;  \
      return 1;                                                                                \
    }                                                                                          \
  } while (0)

int main()
{
  using namespace flexlay;

  // Mirrors tests/test_math.py
  {
    Point p(11, 17);
    CHECK(p.x == 11);
    CHECK(p.y == 17);

    p = 3 * p;
    p = p * 4;
    p *= 5;
    CHECK(p.x == 660);
    CHECK(p.y == 1020);

    p = Point(0, 3) + p;
    p = p + Point(4, 0);
    p += Point(1, 2);
    CHECK(p.x == 665);
    CHECK(p.y == 1025);

    CHECK(Point(11, 17) == Point(11, 17));

    Point p2 = p.copy();
    CHECK(p == p2);
    p2.x += 2;
    p2.y += 3;
    CHECK(p != p2);
  }

  {
    Pointf pf(1.5f, 2.5f);
    CHECK(pf.to_i() == Point(1, 2));
    CHECK(Point(3, 4).to_f() == Pointf(3.0f, 4.0f));
  }

  {
    Size s(10, 20);
    Rect r = Rect::from_ps(Point(5, 7), s);
    CHECK(r.left == 5);
    CHECK(r.top == 7);
    CHECK(r.right == 15);
    CHECK(r.bottom == 27);
    CHECK(r.width() == 10);
    CHECK(r.height() == 20);
    CHECK(r.is_inside(Point(5, 7)));
    CHECK(r.is_inside(Point(15, 27)));
    CHECK(!r.is_inside(Point(4, 7)));
  }

  {
    Rectf rf(10, 10, 5, 20); // inverted left/right
    rf.normalize();
    CHECK(rf.left == 5);
    CHECK(rf.right == 10);
  }

  {
    Point o = calc_origin(Origin::center, Size(10, 20));
    CHECK(o == Point(5, 10));
    Pointf of = calc_originf(Origin::bottom_right, Sizef(10.0f, 20.0f));
    CHECK(of == Pointf(10.0f, 20.0f));
  }

  {
    Color c(1, 2, 3, 4);
    CHECK(c == Color(1, 2, 3, 4));
    CHECK(c.to_hex() == "#010203");
    Colorf cf(1.0f, 0.0f, 0.0f, 1.0f);
    CHECK(cf.to_i() == Color(255, 0, 0, 255));
  }

  std::cout << "test_math: ok\n";
  return 0;
}
