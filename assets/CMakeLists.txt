# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

set(MONDRADIKO_ASSETS_SRC
  Asset.cc
  AssetLump.cc
  AssetRegistry.cc
)

add_library(mondradiko-assets STATIC ${MONDRADIKO_ASSETS_SRC})
target_link_libraries(mondradiko-assets mondradiko-lib)
