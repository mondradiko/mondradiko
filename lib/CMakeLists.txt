# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

#### DEPENDENCIES
include(mondradiko-vcpkg)

find_mondradiko_dependency(mondradiko::lz4 "lz4" lz4::lz4)
find_mondradiko_dependency(mondradiko::xxhash "xxHash" xxHash::xxhash)
find_mondradiko_dependency(mondradiko::glm "glm" glm)
find_mondradiko_dependency(mondradiko::gamenetworkingsockets "GameNetworkingSockets" GameNetworkingSockets::GameNetworkingSockets)
find_mondradiko_dependency(mondradiko::wasmtime "wasmtime" INSTALL "wasmtime-prebuilt" wasmtime::wasmtime)
find_mondradiko_dependency(mondradiko::flatbuffers "Flatbuffers" flatbuffers::flatbuffers)
find_mondradiko_dependency(mondradiko::vulkan "Vulkan" NOCONFIG INSTALL "vulkan-sdk" Vulkan::Vulkan)

find_package(Threads REQUIRED)

### /DEPENDENCIES

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(MONDRADIKO_LIB_SRC library_implementation.cc)

if(${TRACY_ENABLE})
  list(APPEND MONDRADIKO_LIB_SRC tracy/TracyClient.cpp)
endif()

add_library(mondradiko-lib STATIC ${MONDRADIKO_LIB_SRC})

target_link_libraries(mondradiko-lib PUBLIC mondradiko::glm)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::lz4)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::xxhash)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::gamenetworkingsockets)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::wasmtime)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::flatbuffers)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::vulkan)

if(TRACY_ENABLE)
  if(UNIX)
    target_link_libraries(mondradiko-lib PUBLIC dl)
    target_link_libraries(mondradiko-lib PUBLIC pthread)
  endif()
endif()

# Add C++ std::filesystem
if (NOT MSVC)
   target_link_libraries(mondradiko-lib PUBLIC stdc++fs)
endif()

set_target_properties(mondradiko-lib PROPERTIES FOLDER "components")
