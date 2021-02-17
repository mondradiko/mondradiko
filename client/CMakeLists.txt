# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

include(FindMondradikoDependency)
find_mondradiko_dependency(mondradiko::cli11 VCPKG "CLI11" VCPKG_MODULES CLI11::CLI11)

add_executable(mondradiko-client client_main.cc)
target_link_libraries(mondradiko-client mondradiko-core)
target_link_libraries(mondradiko-client mondradiko::cli11)

mondradiko_instrument_exe_runtime_dlls(mondradiko-client)
