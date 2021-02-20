# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

#
# Generate and include headers
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(shaders)
spirv_shaders(SHADER_HEADERS
  shaders/debug.frag
  shaders/debug.vert
  shaders/glyph.frag
  shaders/glyph.vert
  shaders/mesh.frag
  shaders/mesh.vert
  shaders/panel.frag
  shaders/panel.vert
)

#
# Build core source
#
set(MONDRADIKO_CORE_SRC
  assets/MaterialAsset.cc
  assets/MeshAsset.cc
  assets/PrefabAsset.cc
  assets/ScriptAsset.cc
  assets/TextureAsset.cc
  components/MeshRendererComponent.cc
  components/PointLightComponent.cc
  components/ScriptComponent.cc
  components/TransformComponent.cc
  cvars/CVarScope.cc
  displays/OpenXrDisplay.cc
  displays/OpenXrViewport.cc
  displays/SdlDisplay.cc
  displays/SdlViewport.cc
  displays/Viewport.cc
  filesystem/AssetBundle.cc
  filesystem/AssetLump.cc
  filesystem/Filesystem.cc
  gpu/GpuBuffer.cc
  gpu/GpuDescriptorPool.cc
  gpu/GpuDescriptorSet.cc
  gpu/GpuDescriptorSetLayout.cc
  gpu/GpuImage.cc
  gpu/GpuInstance.cc
  gpu/GpuPipeline.cc
  gpu/GpuShader.cc
  renderer/MeshPass.cc
  renderer/OverlayPass.cc
  renderer/Renderer.cc
  scripting/ComponentScript.cc
  scripting/ScriptEnvironment.cc
  scripting/ScriptInstance.cc
  network/NetworkClient.cc
  network/NetworkServer.cc
  ui/GlyphLoader.cc
  ui/UiPanel.cc
  ui/UserInterface.cc
  world/World.cc
  world/WorldEventSorter.cc
)

add_library(mondradiko-core STATIC $<TARGET_OBJECTS:mondradiko-api-components> ${SHADER_HEADERS} ${MONDRADIKO_CORE_SRC})

target_link_libraries(mondradiko-core mondradiko-assets)
target_link_libraries(mondradiko-core mondradiko-log)
target_link_libraries(mondradiko-core mondradiko-lib)
