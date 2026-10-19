// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/commands/command_group.hpp"
#include "flexlay/supertux/object_sprites.hpp"

#include <iostream>

#define CHECK(cond) \
  do { if (!(cond)) { std::cerr << "CHECK failed: " #cond "\n"; return 1; } } while (0)

namespace {

class CounterCommand : public flexlay::Command
{
public:
  explicit CounterCommand(int* c, int delta) : m_c(c), m_delta(delta) {}
  void execute() override { *m_c += m_delta; }
  void undo() override { *m_c -= m_delta; }
  std::string name() const override { return "Counter"; }

private:
  int* m_c;
  int m_delta;
};

} // namespace

int main()
{
  int v = 0;
  auto group = std::make_shared<flexlay::CommandGroup>("group");
  group->add(std::make_shared<CounterCommand>(&v, 1));
  group->add(std::make_shared<CounterCommand>(&v, 10));
  group->execute();
  CHECK(v == 11);
  group->undo();
  CHECK(v == 0);
  group->redo();
  CHECK(v == 11);

  auto info = flexlay::supertux::object_visual_info("snowball");
  CHECK(info.has_value());
  CHECK(info->editor_image.find("snowball") != std::string::npos);

  std::cout << "test_command_group: ok\n";
  return 0;
}
