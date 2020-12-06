# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(THIRD_PARTY_SRC
  lib/library_implementation.cc
  tracy/TracyClient.cpp
)

add_library(third_party STATIC ${THIRD_PARTY_SRC})

# find_package(assimp REQUIRED)
# target_link_libraries(third_party assimp::assimp)
