# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

#
# Generate and include headers
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(shaders)
spirv_shaders(SHADER_HEADERS
  shaders/composite.vert
  shaders/composite.frag
  shaders/debug.frag
  shaders/debug.vert
  shaders/glyph.frag
  shaders/glyph.vert
  shaders/mesh_depth.frag
  shaders/mesh_depth.vert
  shaders/mesh_forward.frag
  shaders/mesh_forward.vert
  shaders/panel.frag
  shaders/panel.vert
  shaders/ui_draw.frag
  shaders/ui_draw.vert
)

#
# Build core source
#
set(MONDRADIKO_CORE_SRC
  assets/Asset.cc
  assets/MaterialAsset.cc
  assets/MeshAsset.cc
  assets/PrefabAsset.cc
  assets/ScriptAsset.cc
  assets/ShapeAsset.cc
  assets/TextureAsset.cc
  avatars/Avatar.cc
  avatars/SpectatorAvatar.cc
  components/internal/ScriptComponent.cc
  components/scriptable/PointLightComponent.cc
  components/scriptable/TransformComponent.cc
  components/synchronized/MeshRendererComponent.cc
  components/synchronized/RelationshipComponent.cc
  components/synchronized/RigidBodyComponent.cc
  cvars/CVarScope.cc
  displays/Display.cc
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
  gpu/GraphicsState.cc
  physics/Physics.cc
  renderer/CompositePass.cc
  renderer/DebugDrawList.cc
  renderer/MeshPass.cc
  renderer/OverlayPass.cc
  renderer/Renderer.cc
  scripting/environment/ComponentScriptEnvironment.cc
  scripting/environment/ScriptEnvironment.cc
  scripting/environment/UiScriptEnvironment.cc
  scripting/environment/WorldScriptEnvironment.cc
  scripting/instance/ComponentScript.cc
  scripting/instance/ScriptInstance.cc
  scripting/instance/UiScript.cc
  network/NetworkClient.cc
  network/NetworkServer.cc
  shapes/AnyShape.cc
  shapes/SphereShape.cc
  ui/glyph/GlyphLoader.cc
  ui/glyph/GlyphStyle.cc
  ui/panels/UiPanel.cc
  ui/UiDrawList.cc
  ui/UserInterface.cc
  world/ScriptEntity.cc
  world/World.cc
  world/WorldEventSorter.cc
)

#### DEPENDENCIES
include(mondradiko-vcpkg)

find_mondradiko_dependency(mondradiko::openxr "OpenXR" INSTALL "openxr-loader" OpenXR::headers OpenXR::openxr_loader OpenXR::openxr-all-supported)

find_mondradiko_dependency(mondradiko::freetype "freetype" freetype)
add_library(Freetype::Freetype ALIAS freetype) # needed because vcpkg's msdfgen looks for Freetype::Freetype
find_mondradiko_dependency(mondradiko::msdfgen "msdfgen" msdfgen::msdfgen msdfgen::msdfgen-ext)

find_mondradiko_dependency(
  mondradiko::sdl2 "SDL2" SDL2::SDL2main
  INSTALL "sdl2[vulkan]"
  UNIX SDL2::SDL2-static
  WIN32 SDL2::SDL2
)

find_mondradiko_dependency(
  mondradiko::bullet "Bullet"
  INSTALL bullet3
  INCLUDE "bullet"
  UNIX -Wl,-Bstatic -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -Wl,-Bdynamic
  WIN32 BulletSoftBody BulletDynamics BulletCollision LinearMath
 )

 find_mondradiko_dependency(mondradiko::zeromq "ZeroMQ" WIN32 libzmq UNIX libzmq-static)

#### /DEPENDENCIES

add_library(mondradiko-core STATIC $<TARGET_OBJECTS:mondradiko-wasm-linker> ${SHADER_HEADERS} ${MONDRADIKO_CORE_SRC})

target_link_libraries(mondradiko-core PUBLIC mondradiko-lib)
target_link_libraries(mondradiko-core PUBLIC mondradiko-log)
target_link_libraries(mondradiko-core PUBLIC mondradiko-types)
target_link_libraries(mondradiko-core PUBLIC mondradiko::sdl2)
target_link_libraries(mondradiko-core PUBLIC mondradiko::openxr)
target_link_libraries(mondradiko-core PUBLIC mondradiko::msdfgen)
target_link_libraries(mondradiko-core PUBLIC mondradiko::bullet)
target_link_libraries(mondradiko-core PUBLIC mondradiko::zeromq)

set_target_properties(mondradiko-core PROPERTIES FOLDER "components")
