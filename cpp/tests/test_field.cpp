// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/field.hpp"

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

  Field f = Field::from_size(4, 3);
  CHECK(f.width() == 4);
  CHECK(f.height() == 3);
  CHECK(f.size() == 12);
  CHECK(f.at(0, 0) == 0);

  f.put(1, 2, 42);
  CHECK(f.at(1, 2) == 42);
  CHECK(f.contains(42));
  CHECK(!f.contains(99));

  Field c = f.copy();
  CHECK(c.at(1, 2) == 42);
  c.put(1, 2, 7);
  CHECK(f.at(1, 2) == 42);
  CHECK(c.at(1, 2) == 7);

  Field region = f.copy_region(0, 0, 2, 2);
  CHECK(region.width() == 2);
  CHECK(region.height() == 2);
  CHECK(region.at(0, 0) == 0);

  Field from_list = Field::from_list(2, 2, {1, 2, 3, 4});
  CHECK(from_list.at(0, 0) == 1);
  CHECK(from_list.at(1, 0) == 2);
  CHECK(from_list.at(0, 1) == 3);
  CHECK(from_list.at(1, 1) == 4);

  f.resize(2, 2, 0, 1);
  CHECK(f.width() == 2);
  CHECK(f.height() == 2);
  // row y=1 of original 4x3: [0,42,0,0] → region at (0,1) size 2x2 starts filling from offset
  // copy_region(x=0,y=1,w=2,h=2): start_y=max(0,-1)=0 ... matches Python semantics

  std::cout << "test_field: ok\n";
  return 0;
}
