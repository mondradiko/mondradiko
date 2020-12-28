# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(PkgConfig REQUIRED)
pkg_check_modules(ASSIMP REQUIRED assimp)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(OPENXR REQUIRED openxr)
pkg_check_modules(XXHASH REQUIRED libxxhash)
pkg_check_modules(LZ4 REQUIRED liblz4)

find_library(WASMTIME wasmtime REQUIRED)
if(NOT WASMTIME)
	message(FATAL_ERROR "wasmtime C API is required - source maintained at https://github.com/bytecodealliance/wasmtime")
endif()

find_package(GameNetworkingSockets REQUIRED)
find_package(Threads REQUIRED)
find_package(Ktx REQUIRED COMPONENTS ktx)

include(FindVulkan)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(MONDRADIKO_LIB_SRC
  library_implementation.cc
  tracy/TracyClient.cpp
)

add_library(mondradiko-lib STATIC ${MONDRADIKO_LIB_SRC})

# Add C++ std::filesystem
target_link_libraries(mondradiko-lib PUBLIC stdc++fs)

target_link_libraries(mondradiko-lib PUBLIC ${ASSIMP_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${ASSIMP_CFLAGS_OTHER})

target_link_libraries(mondradiko-lib PUBLIC ${SDL2_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${SDL2_CFLAGS_OTHER})

target_link_libraries(mondradiko-lib PUBLIC ${OPENXR_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${OPENXR_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${OPENXR_CFLAGS_OTHER})

target_link_libraries(mondradiko-lib PUBLIC ${XXHASH_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${XXHASH_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${XXHASH_CFLAGS_OTHER})

target_link_libraries(mondradiko-lib PUBLIC ${LZ4_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${LZ4_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${LZ4_CFLAGS_OTHER})

target_link_libraries(mondradiko-lib PUBLIC ${WASMTIME})
target_link_libraries(mondradiko-lib PUBLIC GameNetworkingSockets::GameNetworkingSockets_s)
target_link_libraries(mondradiko-lib PUBLIC KTX::ktx)

target_link_libraries(mondradiko-lib PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(mondradiko-lib PUBLIC ${Vulkan_INCLUDE_DIRS})
target_compile_options(mondradiko-lib PUBLIC ${Vulkan_CFLAGS_OTHER})
