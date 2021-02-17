# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

include(FindMondradikoDependency)

find_mondradiko_dependency(
  mondradiko::lz4
  PKG "LZ4" PKG_MODULES liblz4>=1.8.0
  FALLBACK VCPKG "lz4" VCPKG_MODULES lz4::lz4
)

find_mondradiko_dependency(
  mondradiko::openxr
  PKG "OPENXR" PKG_MODULES openxr
  FALLBACK VCPKG "OpenXR" VCPKG_MODULES OpenXR::headers OpenXR::openxr_loader OpenXR::openxr-all-supported
)

find_mondradiko_dependency(
  mondradiko::xxhash
  PKG "XXHASH" PKG_MODULES libxxhash
  FALLBACK VCPKG "xxHash" VCPKG_MODULES xxHash::xxhash
)

# support use of inline / submodule based msdfgen
# (note: user needs to manually install zlib, freetype and openssl in this case)
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/msdfgen)
  message(STATUS "@local[msdfgen]: ${CMAKE_CURRENT_SOURCE_DIR}/msdfgen")
  if (WIN32)
    # TODO(humbletim) - figure how to collapse these into generic find_packages
    set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include)
    set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/zlib.lib)
    #find_mondradiko_dependency(mondradiko::freetype VCPKG "freetype" VCPKG_MODULES Freetype::Freetype)
    set(FREETYPE_LIBRARY ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/freetype.lib)
    set(FREETYPE_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include)
    set(OPENSSL_ROOT_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows)
  endif()
  add_subdirectory(msdfgen)
  add_library(mondradiko::msdfgen INTERFACE IMPORTED)
  set_property(TARGET mondradiko::msdfgen PROPERTY INTERFACE_LINK_LIBRARIES msdfgen::msdfgen msdfgen::msdfgen-ext)
else()
  find_mondradiko_dependency(
    mondradiko::msdfgen
    VCPKG "msdfgen" VCPKG_MODULES msdfgen::msdfgen msdfgen::msdfgen-ext
  )
endif()

find_mondradiko_dependency(
  mondradiko::gamenetworkingsockets
  VCPKG "GameNetworkingSockets" VCPKG_MODULES GameNetworkingSockets::GameNetworkingSockets
)

find_mondradiko_dependency(
  mondradiko::wasmtime
  VCPKG "wasmtime" VCPKG_MODULES wasmtime::wasmtime
)

if (UNIX)
  set(_SDL_SDL SDL2::SDL2-static)
else()
  set(_SDL_SDL SDL2::SDL2)
endif()
find_mondradiko_dependency(
  mondradiko::sdl2
  PKG "SDL2" PKG_MODULES sdl2>=2.0.6
  FALLBACK VCPKG "SDL2" VCPKG_MODULES SDL2::SDL2main ${_SDL_SDL}
)


find_mondradiko_dependency(
  mondradiko::glm
  PKG "GLM" PKG_MODULES glm
  FALLBACK VCPKG "glm"
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
