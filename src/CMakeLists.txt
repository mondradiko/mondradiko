# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories(core)

configure_file(build_config.h.in build_config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(assets)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(launcher)
add_subdirectory(server)
