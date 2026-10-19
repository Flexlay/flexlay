# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(flexlay-simpleed main.cpp)
set_target_properties(flexlay-simpleed PROPERTIES AUTOMOC ON)
# Qt is linked PUBLIC by flexlay when FLEXLAY_BUILD_SIMPLEED is ON.
target_link_libraries(flexlay-simpleed PRIVATE flexlay::flexlay)
