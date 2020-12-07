# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(PkgConfig REQUIRED)
pkg_check_modules(ASSIMP REQUIRED assimp)
pkg_check_modules(PHYSFS REQUIRED physfs)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(OPENXR REQUIRED openxr)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(THIRD_PARTY_SRC
  lib/library_implementation.cc
  tracy/TracyClient.cpp
)

add_library(third_party STATIC ${THIRD_PARTY_SRC})

target_link_libraries(third_party PUBLIC ${ASSIMP_LIBRARIES})
target_include_directories(third_party PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_compile_options(third_party PUBLIC ${ASSIMP_CFLAGS_OTHER})

target_link_libraries(third_party PUBLIC ${PHYSFS_LIBRARIES})
target_include_directories(third_party PUBLIC ${PHYSFS_INCLUDE_DIRS})
target_compile_options(third_party PUBLIC ${PHYSFS_CFLAGS_OTHER})

target_link_libraries(third_party PUBLIC ${SDL2_LIBRARIES})
target_include_directories(third_party PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(third_party PUBLIC ${SDL2_CFLAGS_OTHER})

target_link_libraries(third_party PUBLIC ${OPENXR_LIBRARIES})
target_include_directories(third_party PUBLIC ${OPENXR_INCLUDE_DIRS})
target_compile_options(third_party PUBLIC ${OPENXR_CFLAGS_OTHER})
