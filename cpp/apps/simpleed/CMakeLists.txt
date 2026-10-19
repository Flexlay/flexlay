# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(flexlay-simpleed main.cpp)
# Qt is linked PUBLIC by flexlay when FLEXLAY_BUILD_SIMPLEED is ON; do not
# re-list Qt6::* here (they may not be in this directory's target scope).
target_link_libraries(flexlay-simpleed PRIVATE flexlay::flexlay)
