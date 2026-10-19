# Flexlay - A Generic 2D Game Editor
# Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

cmake_minimum_required(VERSION 3.16)
project(flexlay VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLEXLAY_BUILD_TESTS "Build unit tests" ON)
option(FLEXLAY_BUILD_SIMPLEED "Build the simpleed demo application" ON)

add_subdirectory(libflexlay)

if(FLEXLAY_BUILD_SIMPLEED)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)
  add_subdirectory(apps/simpleed)
endif()

if(FLEXLAY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
