# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

set(FLEXLAY_CORE_SOURCES
  src/field.cpp
  src/tilemap_layer.cpp
  src/editor_map.cpp
)

add_library(flexlay STATIC ${FLEXLAY_CORE_SOURCES})
add_library(flexlay::flexlay ALIAS flexlay)

target_include_directories(flexlay
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(flexlay PUBLIC cxx_std_17)

if(FLEXLAY_BUILD_SIMPLEED)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
  set(CMAKE_AUTOMOC ON)
  target_sources(flexlay PRIVATE src/editor_map_widget.cpp)
  target_link_libraries(flexlay PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
  target_compile_definitions(flexlay PUBLIC FLEXLAY_HAS_QT=1)
endif()
