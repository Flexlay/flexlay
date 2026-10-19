# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(FLEXLAY_HEADERS
  include/flexlay/math/point.hpp
  include/flexlay/math/size.hpp
  include/flexlay/math/rect.hpp
  include/flexlay/math/origin.hpp
  include/flexlay/color.hpp
  include/flexlay/field.hpp
  include/flexlay/qt_compat.hpp
)

set(FLEXLAY_SOURCES
  src/field.cpp
)

add_library(flexlay STATIC ${FLEXLAY_SOURCES} ${FLEXLAY_HEADERS})
add_library(flexlay::flexlay ALIAS flexlay)

target_include_directories(flexlay
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Core library does not hard-require Qt; GUI apps and qt_compat.hpp users link Qt.
target_compile_features(flexlay PUBLIC cxx_std_17)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLEXLAY_SOURCES} ${FLEXLAY_HEADERS})
