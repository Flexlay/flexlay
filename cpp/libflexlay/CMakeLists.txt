# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

set(FLEXLAY_CORE_SOURCES
  src/field.cpp
  src/tilemap_layer.cpp
  src/editor_map.cpp
  src/object_layer.cpp
  src/objmap_rect_object.cpp
  src/objmap_sprite_object.cpp
  src/pixel_buffer.cpp
  src/sprite.cpp
  src/commands/paint_command.cpp
  src/commands/object_move_command.cpp
  src/commands/object_add_command.cpp
  src/commands/object_delete_command.cpp
  src/commands/object_transform_command.cpp
  src/tools/tile_paint_tool.cpp
  src/tools/tile_fill_tool.cpp
  src/tools/objmap_select_tool.cpp
)

add_library(flexlay STATIC ${FLEXLAY_CORE_SOURCES})
add_library(flexlay::flexlay ALIAS flexlay)

target_include_directories(flexlay
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(flexlay PUBLIC cxx_std_17)

# Qt is found by the top-level CMakeLists when FLEXLAY_BUILD_SIMPLEED is ON.
if(FLEXLAY_BUILD_SIMPLEED)
  # List the Q_OBJECT header so AUTOMOC generates the meta-object code into
  # this static library (otherwise vtable/map_changed/staticMetaObject are missing).
  target_sources(flexlay PRIVATE
    src/editor_map_widget.cpp
    include/flexlay/editor_map_widget.hpp
  )
  set_target_properties(flexlay PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
  )
  target_link_libraries(flexlay PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
  target_compile_definitions(flexlay PUBLIC FLEXLAY_HAS_QT=1)
endif()
