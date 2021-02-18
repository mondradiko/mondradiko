# NOTE: this file needs to be included _before_ the first project() statement

option(USE_PKGCONFIG "Where possible attempt to resolve dependencies first using pkg-config" OFF)
option(USE_LAZY_VCPKG "Defer installation of VCPKG dependencies until referenced by a CMakeLists.txt" ${USE_PKGCONFIG})
option(BUILD_VULKAN_VALIDATION "Enable Vulkan validation layers via VCPKG" OFF)

if (DEFINED ENV{USE_PKGCONFIG})
  set(USE_PKGCONFIG $ENV{USE_PKGCONFIG})
endif()

function(mondradiko_execute_vcpkg)
  # LIST(JOIN ARGN " " ARGS)
  # message(STATUS "running ${VCPKG_EXEC} --x-install-root=${VCPKG_INSTALL_ROOT} --overlay-ports=${VCPKG_OVERLAY_PORTS} --overlay-triplets=${VCPKG_OVERLAY_TRIPLETS} --triplet=${VCPKG_DEFAULT_TRIPLET} ${ARGS}")
  execute_process(
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${VCPKG_EXEC}
      --x-install-root=${VCPKG_INSTALL_ROOT}
      --overlay-ports=${VCPKG_OVERLAY_PORTS}
      --overlay-triplets=${VCPKG_OVERLAY_TRIPLETS}
      --triplet=${VCPKG_DEFAULT_TRIPLET}
      ${ARGN}
  )
endfunction()

function(mondradiko_bootstrap_vcpkg)
  if (WIN32)
    set(VCPKG_DEFAULT_TRIPLET "x64-windows" CACHE INTERNAL "")
  endif()
  if (UNIX)
    set(VCPKG_DEFAULT_TRIPLET "x64-linux" CACHE INTERNAL "")
  endif()

  set(VCPKG_OVERLAY_TRIPLETS ${CMAKE_SOURCE_DIR}/cmake/vcpkg-triplets CACHE INTERNAL "")
  set(VCPKG_OVERLAY_PORTS ${CMAKE_SOURCE_DIR}/cmake/vcpkg-ports CACHE INTERNAL "")
  set(VCPKG_INSTALL_ROOT ${CMAKE_BINARY_DIR}/vcpkg_installed CACHE INTERNAL "")
  set(VCPKG_INSTALL_TRIPLETROOT "${VCPKG_INSTALL_ROOT}/${VCPKG_DEFAULT_TRIPLET}" CACHE INTERNAL "")

  # inform find_package(), find_program(), find_library() about vcpkg locations
  list(APPEND CMAKE_PREFIX_PATH
    "${VCPKG_INSTALL_TRIPLETROOT}/tools"
    "${VCPKG_INSTALL_TRIPLETROOT}/share"
    "${VCPKG_INSTALL_TRIPLETROOT}/lib"
    "${VCPKG_INSTALL_TRIPLETROOT}/include"
  )
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

  if(WIN32)
    set(VCPKG_EXEC ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg.exe CACHE INTERNAL "")
    set(VCPKG_BOOTSTRAP ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat)
  else()
    set(VCPKG_EXEC ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg CACHE INTERNAL "")
    set(VCPKG_BOOTSTRAP ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message("bootstrapping vcpkg in ${CMAKE_SOURCE_DIR}/vcpkg")
    execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg)
  endif()

  if(NOT EXISTS ${VCPKG_EXEC})
    message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap ${VCPKG_EXEC} using ${VCPKG_BOOTSTRAP} -- check compiler version and vcpkg git submodule (did you clone recursive?)*****")
  endif()
  
  # wire-up the vcpkg cmake toolchain file
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

  # create a helper script for users to troubleshoot VCPKG package installs with
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/vcpkg.sh.in
    ${CMAKE_BINARY_DIR}/vcpkg.sh
    @ONLY
  )

  message(STATUS "VCPKG MODE ENABLED =========================================")
  message(STATUS "     overlay-ports: SRC/cmake/vcpkg-ports")
  message(STATUS "  overlay-triplets: SRC/cmake/vcpkg-triplets")
  message(STATUS "      installed to: BIN/vcpkg_installed")
  message(STATUS " troubleshoot with: BIN/vcpkg.sh")
  message(STATUS "    tripleted root: ${VCPKG_INSTALL_TRIPLETROOT}")
  if (USE_LAZY_VCPKG)
    message(STATUS "    USE_LAZY_VCPKG: ${USE_LAZY_VCPKG} (install packages on-demand)")
  else()
    message(STATUS "    USE_LAZY_VCPKG: ${USE_LAZY_VCPKG} (modradiko-deps upfront install)")
  endif()
  message(STATUS "============================================================")
endfunction()

# main vcpkg bootstrapping
mondradiko_bootstrap_vcpkg()

if (USE_PKGCONFIG)
  message(STATUS "USE_PKGCONFIG == ${USE_PKGCONFIG}")
endif()

if (NOT USE_PKGCONFIG AND NOT USE_LAZY_VCPKG)
  mondradiko_execute_vcpkg(install mondradiko-deps)
endif()

if (BUILD_VULKAN_VALIDATION)
  mondradiko_execute_vcpkg(install vulkan-validation)
  message(">> To enable in-tree vulkan validation layers:\n\tset VK_LAYER_PATH=${VCPKG_INSTALL_TRIPLETROOT}/bin\n\t(before launching mondradiko-client.exe)")
endif()

