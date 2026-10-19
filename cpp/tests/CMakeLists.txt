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

add_executable(test_paint test_paint.cpp)
target_link_libraries(test_paint PRIVATE flexlay::flexlay)
add_test(NAME test_paint COMMAND test_paint)

add_executable(test_objects test_objects.cpp)
target_link_libraries(test_objects PRIVATE flexlay::flexlay)
add_test(NAME test_objects COMMAND test_objects)

add_executable(test_sprite test_sprite.cpp)
target_link_libraries(test_sprite PRIVATE flexlay::flexlay)
add_test(NAME test_sprite COMMAND test_sprite)

add_executable(test_level_loader test_level_loader.cpp)
target_link_libraries(test_level_loader PRIVATE flexlay::flexlay)
add_test(NAME test_level_loader COMMAND test_level_loader)

add_executable(test_tileset_loader test_tileset_loader.cpp)
target_link_libraries(test_tileset_loader PRIVATE flexlay::flexlay)
add_test(NAME test_tileset_loader COMMAND test_tileset_loader)
