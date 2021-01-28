# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

add_executable(mondradiko-server server_main.cc)
target_link_libraries(mondradiko-server mondradiko-core)
