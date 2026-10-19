# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(flexlay-simpleed main.cpp)
target_link_libraries(flexlay-simpleed PRIVATE flexlay::flexlay Qt6::Core Qt6::Gui Qt6::Widgets)
