# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

spirv_shaders(SHADER_HEADERS
  shaders/mesh.frag
  shaders/mesh.vert
)

# Generate and include shaders
add_subdirectory(shaders)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MONDRADIKO_CORE_SRC
  assets/MaterialAsset.cc
  assets/MeshAsset.cc
  assets/TextureAsset.cc
  displays/OpenXrDisplay.cc
  displays/OpenXrViewport.cc
  displays/SdlDisplay.cc
  displays/SdlViewport.cc
  filesystem/AssimpIOStream.cc
  filesystem/AssimpIOSystem.cc
  filesystem/Filesystem.cc
  gpu/GpuBuffer.cc
  gpu/GpuDescriptorPool.cc
  gpu/GpuDescriptorSet.cc
  gpu/GpuDescriptorSetLayout.cc
  gpu/GpuImage.cc
  gpu/GpuInstance.cc
  gpu/GpuShader.cc
  renderer/MeshPipeline.cc
  renderer/Renderer.cc
  log/log.cc
  network/NetworkClient.cc
  network/NetworkShared.cc
  scene/Scene.cc
)

add_library(mondradiko-core STATIC ${SHADER_HEADERS} ${MONDRADIKO_CORE_SRC})
target_link_libraries(mondradiko-core third_party)

include(FindVulkan)
target_link_libraries(mondradiko-core ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
