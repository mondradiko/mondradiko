# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(MONDRADIKO_ASSETS_SRC
  common/AssetTypes.cc
)

add_compile_definitions(MONDRADIKO_ASSET_VERSION=0)

add_library(mondradiko-assets STATIC ${FORMAT_HEADERS} ${MONDRADIKO_ASSETS_SRC})
target_link_libraries(mondradiko-assets mondradiko-lib)
target_link_libraries(mondradiko-assets mondradiko-log)
