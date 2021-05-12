# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(CONVERTER_SRC
  prefab/BinaryGltfConverter.cc
  prefab/GltfConverter.cc
  prefab/PrefabBuilder.cc
  prefab/TextGltfConverter.cc
  script/WasmConverter.cc
  AssetBundleBuilder.cc
)

add_library(mondradiko-converter STATIC ${CONVERTER_SRC})
target_link_libraries(mondradiko-converter mondradiko-types)
target_link_libraries(mondradiko-converter mondradiko-lib)
target_link_libraries(mondradiko-converter mondradiko-log)

set_target_properties(mondradiko-converter PROPERTIES FOLDER "components")
