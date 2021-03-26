# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

configure_file(build_config.h.in build_config.h)

add_subdirectory(assets)
flatc_schemas(ASSET_HEADERS
  assets/MaterialAsset.fbs
  assets/MeshAsset.fbs
  assets/PrefabAsset.fbs
  assets/Registry.fbs
  assets/ScriptAsset.fbs
  assets/SerializedAsset.fbs
  assets/TextureAsset.fbs
  assets/types.fbs
)

add_subdirectory(protocol)
flatc_schemas(PROTOCOL_HEADERS
  protocol/ClientEvent.fbs
  protocol/MeshRendererComponent.fbs
  protocol/PointLightComponent.fbs
  protocol/RelationshipComponent.fbs
  protocol/ServerEvent.fbs
  protocol/TransformComponent.fbs
  protocol/ScriptData.fbs
  protocol/WorldEvent.fbs
  protocol/types.fbs
)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19.0")
  # see: https://cmake.org/cmake/help/v3.19/command/add_library.html#interface-libraries
  add_library(mondradiko-types INTERFACE ${ASSET_HEADERS} ${PROTOCOL_HEADERS})
else()
  add_library(mondradiko-types STATIC ${ASSET_HEADERS} ${PROTOCOL_HEADERS})
  set_target_properties(mondradiko-types PROPERTIES LINKER_LANGUAGE CXX)
endif()
