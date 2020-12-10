# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.19)

project(Mondradiko VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SPIR-V)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(lib)
add_subdirectory(assets)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(launcher)
add_subdirectory(server)
add_subdirectory(tools)
