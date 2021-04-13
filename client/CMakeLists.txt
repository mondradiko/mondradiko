# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

include(mondradiko-vcpkg)
find_mondradiko_dependency(mondradiko::cli11 "CLI11" CLI11::CLI11)

add_executable(mondradiko-client client_main.cc)
target_link_libraries(mondradiko-client mondradiko-core)
target_link_libraries(mondradiko-client mondradiko::cli11)

mondradiko_instrument_exe_runtime_dlls(mondradiko-client)

set_target_properties(mondradiko-client PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "--serverless --config=../config.toml --bundle=..")
