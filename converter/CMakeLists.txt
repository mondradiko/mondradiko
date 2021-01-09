# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(MONDRADIKO_CONVERTER_SRC
  convert_gltf.cc
  stb_converter.cc
  wasm_converter.cc
)

add_library(mondradiko-converter STATIC ${MONDRADIKO_CONVERTER_SRC})
target_link_libraries(mondradiko-converter mondradiko-assets)
