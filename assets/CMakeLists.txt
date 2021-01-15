# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(format)
flatc_schemas(FORMAT_HEADERS
  format/MaterialAsset.fbs
  format/MeshAsset.fbs
  format/PrefabAsset.fbs
  format/Registry.fbs
  format/ScriptAsset.fbs
  format/SerializedAsset.fbs
  format/TextureAsset.fbs
  format/types.fbs
)

set(MONDRADIKO_ASSETS_SRC
  common/AssetTypes.cc
  loading/AssetBundle.cc
  loading/AssetLump.cc
  saving/AssetBundleBuilder.cc
)

add_compile_definitions(MONDRADIKO_ASSET_VERSION=0)

add_library(mondradiko-assets STATIC ${FORMAT_HEADERS} ${MONDRADIKO_ASSETS_SRC})
target_link_libraries(mondradiko-assets mondradiko-lib)
target_link_libraries(mondradiko-assets mondradiko-log)
