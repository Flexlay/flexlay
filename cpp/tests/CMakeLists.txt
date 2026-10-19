# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(test_math test_math.cpp)
target_link_libraries(test_math PRIVATE flexlay::flexlay)
add_test(NAME test_math COMMAND test_math)

add_executable(test_field test_field.cpp)
target_link_libraries(test_field PRIVATE flexlay::flexlay)
add_test(NAME test_field COMMAND test_field)

add_executable(test_tilemap test_tilemap.cpp)
target_link_libraries(test_tilemap PRIVATE flexlay::flexlay)
add_test(NAME test_tilemap COMMAND test_tilemap)
