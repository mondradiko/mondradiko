# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

#
# Generate and include headers
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(shaders)
spirv_shaders(SHADER_HEADERS
  shaders/debug.frag
  shaders/debug.vert
  shaders/mesh.frag
  shaders/mesh.vert
)

add_subdirectory(protocol)
flatc_schemas(PROTOCOL_HEADERS
  protocol/example.fbs
)

configure_file(build_config.h.in build_config.h)

#
# Build core source
#
set(MONDRADIKO_CORE_SRC
  assets/MaterialAsset.cc
  assets/MeshAsset.cc
  assets/TextureAsset.cc
  displays/OpenXrDisplay.cc
  displays/OpenXrViewport.cc
  displays/SdlDisplay.cc
  displays/SdlViewport.cc
  filesystem/Filesystem.cc
  gpu/GpuBuffer.cc
  gpu/GpuDescriptorPool.cc
  gpu/GpuDescriptorSet.cc
  gpu/GpuDescriptorSetLayout.cc
  gpu/GpuImage.cc
  gpu/GpuInstance.cc
  gpu/GpuShader.cc
  renderer/MeshPass.cc
  renderer/OverlayPass.cc
  renderer/Renderer.cc
  network/NetworkClient.cc
  network/NetworkServer.cc
  scene/Scene.cc
)

add_library(mondradiko-core STATIC ${PROTOCOL_HEADERS} ${SHADER_HEADERS} ${MONDRADIKO_CORE_SRC})
target_link_libraries(mondradiko-core mondradiko-assets)
target_link_libraries(mondradiko-core mondradiko-log)
target_link_libraries(mondradiko-core mondradiko-lib)
