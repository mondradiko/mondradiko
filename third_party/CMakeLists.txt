# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(THIRD_PARTY_SRC
  lib/vk_mem_alloc.h
  lib/tiny_gltf.h
  tracy/TracyClient.cpp
)

add_library(third_party STATIC ${THIRD_PARTY_SRC})
target_compile_definitions(third_party PRIVATE
  VMA_IMPLEMENTATION
  TINYGLTF_IMPLEMENTATION
  STB_IMAGE_IMPLEMENTATION
  STB_IMAGE_WRITE_IMPLEMENTATION
)

include(FindVulkan)
target_link_libraries(third_party ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})

# find_package(assimp REQUIRED)
# target_link_libraries(third_party assimp::assimp)
