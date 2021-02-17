# enable VCPKG dependencies (unless ENV{USE_VCPKG}=OFF or -DUSE_VCPKG=OFF)
# vcpkg deps are managed through cmake/vcpkg-ports/mondradiko-deps/vcpkg.json

# NOTE: this file needs to be included _before_ the first project() statement

option(USE_VCPKG "Enable automated VCPKG dependencies" ON)
option(BUILD_VULKAN_VALIDATION "Enable Vulkan validation layers via VCPKG" OFF)

if (DEFINED ENV{USE_VCPKG})
  set(USE_VCPKG $ENV{USE_VCPKG})
endif()

macro(enable_vcpkg_support)
  # enable vcpkg manifest mode
  #set(VCPKG_FEATURE_FLAGS manifests)

  # make sure x64 is used on windows (vcpkg otherwise defaults to x86-windows)
  if (WIN32)
    set(VCPKG_DEFAULT_TRIPLET "x64-windows")
  endif()
  if (UNIX)
    set(VCPKG_DEFAULT_TRIPLET "x64-linux")
  endif()

  # enable use of our custom triplet overlays
  #set(ENV{VCPKG_OVERLAY_TRIPLETS} ${CMAKE_CURRENT_LIST_DIR}/vcpkg-triplets)
  set(VCPKG_OVERLAY_TRIPLETS ${CMAKE_SOURCE_DIR}/cmake/vcpkg-triplets)
  # enable use of our custom dependency portfiles
  set(VCPKG_OVERLAY_PORTS ${CMAKE_SOURCE_DIR}/cmake/vcpkg-ports)

  set(_VCPKG_INSTALLED_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed)

  if(WIN32)
    set(VCPKG_EXEC ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg.exe)
    set(VCPKG_BOOTSTRAP ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat)
  else()
    set(VCPKG_EXEC ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg)
    set(VCPKG_BOOTSTRAP ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message("bootstrapping vcpkg in ${CMAKE_SOURCE_DIR}/vcpkg")
    execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg using ${VCPKG_BOOTSTRAP} -- check compiler version and vcpkg git submodule (did you clone recursive?)*****")
  endif()
  
  if (BUILD_VULKAN_VALIDATION)
    set(VULKAN_VALIDATION_DEP vulkan-validation)
    message(">> To enable in-tree vulkan validation layers:\n\tset VK_LAYER_PATH=${_VCPKG_INSTALLED_DIR}/${VCPKG_DEFAULT_TRIPLET}/bin\n\t(before launching mondradiko-client.exe)")
  else()
    set(VULKAN_VALIDATION_DEP "")
  endif()
  message(STATUS "running vcpkg --x-install-root=${_VCPKG_INSTALLED_DIR} --overlay-ports=${VCPKG_OVERLAY_PORTS} --overlay-triplets=${VCPKG_OVERLAY_TRIPLETS} --triplet=${VCPKG_DEFAULT_TRIPLET}")
  execute_process(
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${VCPKG_EXEC}
      --x-install-root=${_VCPKG_INSTALLED_DIR}
      --overlay-ports=${VCPKG_OVERLAY_PORTS}
      --overlay-triplets=${VCPKG_OVERLAY_TRIPLETS}
      --triplet=${VCPKG_DEFAULT_TRIPLET}
      install mondradiko-deps ${VULKAN_VALIDATION_DEP}
  )

  # bootstrap the vcpkg toolchain file
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

  message(STATUS "VCPKG MODE ENABLED =========================================")
  message(STATUS "     overlay-ports: ${CMAKE_SOURCE_DIR}/cmake/vcpkg-ports")
  message(STATUS "  overlay-triplets: ${CMAKE_SOURCE_DIR}/cmake/vcpkg-triplets")
  message(STATUS "      installed to: ${CMAKE_BINARY_DIR}/vcpkg_installed")
  message(STATUS "============================================================")

  #include(${CMAKE_TOOLCHAIN_FILE})
endmacro()

if (USE_VCPKG)
  enable_vcpkg_support()
endif()
