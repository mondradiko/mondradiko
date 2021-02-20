# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

include(FindMondradikoDependency)

find_mondradiko_dependency(
  mondradiko::lz4
  PKG "LZ4" PKG_MODULES liblz4>=1.8.0
  VCPKG "lz4" VCPKG_MODULES lz4::lz4
)

find_mondradiko_dependency(
  mondradiko::openxr
  PKG "OPENXR" PKG_MODULES openxr
  VCPKG "OpenXR" VCPKG_INSTALL "openxr-loader" VCPKG_MODULES OpenXR::headers OpenXR::openxr_loader OpenXR::openxr-all-supported
)

find_mondradiko_dependency(
  mondradiko::xxhash
  PKG "XXHASH" PKG_MODULES libxxhash
  VCPKG "xxHash" VCPKG_MODULES xxHash::xxhash
)

if (UNIX)
  set(_SDL_SDL SDL2::SDL2-static)
else()
  set(_SDL_SDL SDL2::SDL2)
endif()
find_mondradiko_dependency(
  mondradiko::sdl2
  PKG "SDL2" PKG_MODULES sdl2>=2.0.6
  VCPKG "SDL2" VCPKG_INSTALL "sdl2[vulkan]" VCPKG_MODULES SDL2::SDL2main ${_SDL_SDL}
)

find_mondradiko_dependency(
  mondradiko::glm
  PKG "GLM" PKG_MODULES glm
  VCPKG "glm" VCPKG_MODULES glm
)

find_mondradiko_dependency(
  mondradiko::msdfgen
  VCPKG "msdfgen" VCPKG_MODULES msdfgen::msdfgen msdfgen::msdfgen-ext
)

find_mondradiko_dependency(
  mondradiko::gamenetworkingsockets
  VCPKG "GameNetworkingSockets" VCPKG_MODULES GameNetworkingSockets::GameNetworkingSockets
)

find_mondradiko_dependency(
  mondradiko::wasmtime
  VCPKG "wasmtime" VCPKG_INSTALL "wasmtime-prebuilt" VCPKG_MODULES wasmtime::wasmtime
)

find_mondradiko_dependency(
  mondradiko::flatbuffers
  VCPKG "Flatbuffers" VCPKG_MODULES flatbuffers::flatbuffers
)

find_package(Vulkan REQUIRED)
add_library(mondradiko::vulkan ALIAS Vulkan::Vulkan)

find_package(Threads REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(MONDRADIKO_LIB_SRC library_implementation.cc)

if(${TRACY_ENABLE})
  list(APPEND MONDRADIKO_LIB_SRC tracy/TracyClient.cpp)
endif()

add_library(mondradiko-lib STATIC ${MONDRADIKO_LIB_SRC})

target_link_libraries(mondradiko-lib PUBLIC mondradiko::glm)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::lz4)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::openxr)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::xxhash)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::gamenetworkingsockets)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::wasmtime)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::sdl2)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::flatbuffers)
target_link_libraries(mondradiko-lib PUBLIC mondradiko::msdfgen)
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
