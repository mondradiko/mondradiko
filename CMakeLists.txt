# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.13)
project(Mondradiko VERSION 0.0.0)

option(TRACY_ENABLE "Enable Tracy profiling." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(api_bindings)
include(SPIR-V)
include(flatc)

# Enable Tracy profiling
if(${TRACY_ENABLE})
  add_compile_definitions(TRACY_ENABLE)
endif()

# Copy runtime config into binary folder
configure_file(example-config.toml config.toml COPYONLY)

# Let all parts of the source base access each other's headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
# Let all parts of the source base access each other's generated files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(lib)

add_compile_options(-Wall)

add_subdirectory(log)
add_subdirectory(assets)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(launcher)
add_subdirectory(server)
add_subdirectory(bundler)
