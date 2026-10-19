# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
project(flexlay VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLEXLAY_BUILD_TESTS "Build unit tests" ON)
option(FLEXLAY_BUILD_SIMPLEED "Build the simpleed demo application" ON)

# Find Qt once at the top level so all targets see Qt6::* IMPORTED targets.
if(FLEXLAY_BUILD_SIMPLEED)
  set(CMAKE_AUTOMOC ON)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
endif()

add_subdirectory(libflexlay)

if(FLEXLAY_BUILD_SIMPLEED)
  add_subdirectory(apps/simpleed)
endif()

if(FLEXLAY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Install rules (used by Nix packaging)
include(GNUInstallDirs)
install(TARGETS flexlay
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY libflexlay/include/flexlay
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
if(FLEXLAY_BUILD_SIMPLEED)
  install(TARGETS flexlay-simpleed RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
