// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/objmap_sprite_object.hpp"
#include "flexlay/pixel_buffer.hpp"
#include "flexlay/sprite.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

int main()
{
  using namespace flexlay;

  auto pb = PixelBuffer::from_color(Size(16, 8), Color(255, 0, 0, 255));
  CHECK(pb.width() == 16);
  CHECK(pb.height() == 8);
  CHECK(pb.pixel(0, 0) == Color(255, 0, 0, 255));

  Sprite spr(pb);
  CHECK(spr.width() == 16);
  CHECK(spr.height() == 8);

  ObjMapSpriteObject obj(spr, Pointf(10, 20));
  const Rectf b = obj.get_bound_rect();
  CHECK(b.left == 10);
  CHECK(b.top == 20);
  CHECK(b.width() == 16);
  CHECK(b.height() == 8);

  std::cout << "test_sprite: ok\n";
  return 0;
}
