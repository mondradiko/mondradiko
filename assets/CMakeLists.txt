# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

set(MONDRADIKO_ASSETS_SRC
  AssetBundle.cc
  AssetBundleBuilder.cc
  AssetLump.cc
  AssetTypes.cc
)

add_compile_definitions(MONDRADIKO_ASSET_VERSION=0)

add_library(mondradiko-assets STATIC ${MONDRADIKO_ASSETS_SRC})
target_link_libraries(mondradiko-assets mondradiko-lib)
