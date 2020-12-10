# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

add_executable(mondradiko-bundler bundler_main.cc)
target_link_libraries(mondradiko-bundler mondradiko-core)
