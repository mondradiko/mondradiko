# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(BUNDLER_SRC
  bundler_main.cc
  Bundler.cc
)

include(mondradiko-vcpkg)
find_mondradiko_dependency(mondradiko::cli11 "CLI11" CLI11::CLI11)

add_executable(mondradiko-bundler ${BUNDLER_SRC})
target_link_libraries(mondradiko-bundler mondradiko-converter)
target_link_libraries(mondradiko-bundler mondradiko-log)
target_link_libraries(mondradiko-bundler mondradiko::cli11)

mondradiko_instrument_exe_runtime_dlls(mondradiko-bundler)
