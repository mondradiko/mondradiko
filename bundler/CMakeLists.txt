# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(BUNDLER_SRC
  prefab/BinaryGltfConverter.cc
  prefab/GltfConverter.cc
  prefab/TextGltfConverter.cc
  script/WasmConverter.cc
  AssetBundleBuilder.cc
  bundler_main.cc
  Bundler.cc
  PrefabBuilder.cc
)

add_executable(mondradiko-bundler ${BUNDLER_SRC})
# target_link_libraries(mondradiko-bundler mondradiko-converter)
target_link_libraries(mondradiko-bundler mondradiko-assets)
target_link_libraries(mondradiko-bundler mondradiko-lib)
target_link_libraries(mondradiko-bundler CLI11::CLI11)
