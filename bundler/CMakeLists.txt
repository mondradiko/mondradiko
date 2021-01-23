# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(BUNDLER_SRC
  bundler_main.cc
  Bundler.cc
)

add_executable(mondradiko-bundler ${BUNDLER_SRC})
target_link_libraries(mondradiko-bundler mondradiko-converter)
