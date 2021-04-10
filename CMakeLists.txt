# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later
cmake_minimum_required(VERSION 3.18)

include(cmake/prelude.cmake)

project(Mondradiko VERSION 0.0.0)
set(Mondradiko_COPYRIGHT "Copyright (c) 2020-2021 the Mondradiko contributors.")
set(Mondradiko_LICENSE "SPDX-License-Identifier: LGPL-3.0-or-later")

option(TRACY_ENABLE "Enable Tracy profiling." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

message(STATUS "CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND CMAKE_CXX_FLAGS -Wno-nullability-completeness)
endif()

if (MSVC)
  # needed so that M_PI_2 consts etc. become available with cmath.h / math.h
  add_compile_definitions(_USE_MATH_DEFINES)
endif()

include(SPIR-V)
include(flatc)

# Enable Tracy profiling
if(${TRACY_ENABLE})
  add_compile_definitions(TRACY_ENABLE)
endif()

# Copy config files into binary folder
file(COPY config/config.toml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Let all parts of the source base access each other's headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
# Let all parts of the source base access each other's generated files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(lib)

if (NOT WIN32)
  add_compile_options(-Wall)
endif()

add_subdirectory(log)
add_subdirectory(types)
add_subdirectory(codegen)
add_subdirectory(converter)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(bundler)
