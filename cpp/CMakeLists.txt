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

add_subdirectory(libflexlay)

if(FLEXLAY_BUILD_SIMPLEED)
  set(CMAKE_AUTOMOC ON)
  add_subdirectory(apps/simpleed)
endif()

if(FLEXLAY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
