# NOTE: this file needs to be included _before_ the first project() statement

option(USE_LAZY_VCPKG "Defer installation of VCPKG dependencies until referenced by a CMakeLists.txt" OFF)
option(BUILD_VULKAN_VALIDATION "Enable Vulkan validation layers via VCPKG" OFF)

# find_mondradiko_dependency(
#   <internal alias>
#   <vcpkg-name>
#   <vcpkg module names...>
#   [WIN32 <win32-specific module names>]
#   [UNIX <linux-specific module names>]
#   [INSTALL <vcpkg install name>]
# )
#
# to make a vcpkg dependency available:
#   find_mondradiko_dependency(mondradiko::xxhash "xxHash" xxHash::xxhash)
#
# to use a vcpkg dependency:
#   target_link_libraries(<target_name> PUBLIC mondradiko::xxhash)

function(find_mondradiko_dependency _opts_ALIAS _opts_VCPKG) #_opts_UNPARSED_ARGUMENTS
    if (TARGET ${_opts_ALIAS})
      #message(STATUS "[find_mondradiko_dependency target already defined: ${_opts_ALIAS}]")
      return()
    endif()

    cmake_parse_arguments(_opts "" "INSTALL" "UNIX;WIN32" ${ARGN})  

    if (NOT _opts_INSTALL)
      set(_opts_INSTALL ${_opts_VCPKG})
    endif()

    if (WIN32 AND DEFINED _opts_WIN32)
      list(APPEND _opts_UNPARSED_ARGUMENTS ${_opts_WIN32})
    endif()

    if (UNIX AND DEFINED _opts_UNIX)
      list(APPEND _opts_UNPARSED_ARGUMENTS ${_opts_UNIX})
    endif()

    mondradiko_install_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${_opts_INSTALL} ${_opts_UNPARSED_ARGUMENTS})

    if (NOT TARGET ${_opts_ALIAS})
      message(FATAL_ERROR "ERROR LOCATING DEPENDENCY ${_opts_ALIAS} ${_opts_VCPKG} ${_opts_INSTALL} ${_opts_UNPARSED_ARGUMENTS}")
    endif()
endfunction()

# mondradiko_find_vcpkg (<alias> [REQUIRED] [QUIET] <vcpkg-name> <vcpkg-modules> [INCLUDE <extra include subdir>] [LIBONLY <vcpkg-libraries>])
# ... finds an installed VCPKG package and registers as an alias target
# eg: find_package_vcpkg(mondradiko::sdl2 sdl2 SDL2::SDL2)
function(mondradiko_find_vcpkg _opts_ALIAS _opts_VCPKG)
  cmake_parse_arguments(_opts "REQUIRED;QUIET;NOCONFIG;" "INCLUDE" "LIBONLY" ${ARGN})
  #message(STATUS "find_package_vcpkg -- unparsed: ${_opts_UNPARSED_ARGUMENTS} // REQUIRED=${_opts_REQUIRED} QUIET=${_opts_QUIET} INCLUDE=${_opts_INCLUDE} LIBONLY=${_opts_LIBONLY}")
  set(_ARGS "")
  if (_opts_REQUIRED AND NOT _opts_LIBONLY)
    list(APPEND _ARGS REQUIRED)
  endif()
  if (_opts_QUIET)
    list(APPEND _ARGS QUIET)
  endif()
  if (NOT _opts_NOCONFIG)
    list(PREPEND _ARGS CONFIG)
  endif()

  # LIBONLY mode -- uses cmake find_library to detect dependency
  if (_opts_LIBONLY)
      list(APPEND _opts_UNPARSED_ARGUMENTS ${_opts_LIBONLY})
      find_library(${_opts_VCPKG}_FOUND ${_opts_LIBONLY} HINTS ${VCPKG_INSTALL_TRIPLETROOT}/lib)
  endif()

  # uses cmake find_package to detect
  if (NOT ${_opts_VCPKG}_FOUND)
      #message(STATUS "find_package ${_opts_VCPKG} ${_ARGS}")
      find_package(${_opts_VCPKG} ${_ARGS})
      #find_package(${_opts_VCPKG} ${_ARGS} HINTS ${VCPKG_INSTALL_TRIPLETROOT}/share/${opts_VCPKG}) # NAMES ${opts_VCPKG} )
  endif()

  if (NOT ${_opts_VCPKG}_FOUND)
    if (_opts_REQUIRED)
      message(FATAL_ERROR "(vcpkg[${_opts_VCPKG}]): NOT FOUND ${_opts_ALIAS}")
    else()
      message(STATUS "(vcpkg[${_opts_VCPKG}]): NOT FOUND ${_opts_ALIAS}")
    endif()
    return()
  endif()

  # register an interface library capturing dependency includes and libs
  if (NOT TARGET ${_opts_ALIAS})
    set(_extra_include_dirs)
    foreach(_dir ${_opts_INCLUDE})
      if (IS_ABSOLUTE ${_dir})
        list(APPEND _extra_include_dirs "${_dir}")
      else()
        list(APPEND _extra_include_dirs "${VCPKG_INSTALL_TRIPLETROOT}/include/${_dir}")
      endif()
    endforeach()
    message(STATUS "@vcpkg: ${_opts_ALIAS}")
    add_library(${_opts_ALIAS} INTERFACE IMPORTED GLOBAL)
    set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_LINK_LIBRARIES "${_opts_UNPARSED_ARGUMENTS}")
    set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_INSTALL_TRIPLETROOT}/include" "${_extra_include_dirs}")
    # message(STATUS "CUSTOM EXTRA INCLUDE: ${VCPKG_INSTALL_TRIPLETROOT}/include ${_extra_include_dirs}")
  else()
    message(STATUS "(vcpkg): ${_opts_ALIAS} already exists... not redefining")
  endif()
endfunction()

# mondradiko_install_vcpkg(<alias> <vcpkg-name> <vcpkg-install>)
# ... install if necessary and register a VCPKG dependency
# eg: mondradiko_install_vcpkg(mondradiko::sdl2 sdl2 "sdl2[vulkan]")
function(mondradiko_install_vcpkg _opts_ALIAS _opts_VCPKG _opts_INSTALL)
  if (NOT TARGET ${_opts_ALIAS})
    #message(STATUS "mondradiko_install_vcpkg -- ARGN=${ARGN}")
    mondradiko_find_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${ARGN})
    if (NOT TARGET ${_opts_ALIAS})
      message(STATUS "(vcpkg[${_opts_VCPKG}]): attempting to autoinstall... \"${_opts_VCPKG}\" as \"${_opts_INSTALL}\"")
      mondradiko_execute_vcpkg(install ${_opts_INSTALL})
      mondradiko_find_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} REQUIRED ${ARGN})
      if (NOT TARGET ${_opts_ALIAS})
        message(FATAL_ERROR "unable to find/install package via VCPKG: ${_opts_VCPKG}")
      endif()
    endif()
  endif()
endfunction()

# run vcpkg with custom install root, overlay triplets and overlay ports
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

# setup vckpg global cmake configuration
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
  )
  if (NOT USE_PKGCONFIG)
    list(APPEND CMAKE_PREFIX_PATH
      "${VCPKG_INSTALL_TRIPLETROOT}/lib"
      "${VCPKG_INSTALL_TRIPLETROOT}/bin"
      "${VCPKG_INSTALL_TRIPLETROOT}/include"
    )
  endif()
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

### MISC

function(mondradiko_instrument_exe_runtime_dlls TARGET_NAME)
  if (WIN32)

    # group applications into their own visual studio solution view folder
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "apps")

    # VCPKG_APPLOCAL_DEPS works fine to ensure runtime DLLs get copied for Release
    # builds, but not in the case of compiling the project in Debug mode while
    # still linking against VCPKG Release dependencies.
    #
    # This custom command replicates the same effect so Visual Studio users
    # can build an application .exe in Debug mode and simply hit F5 to start
    # debugging it (without first having to manually shuffle DLLs around).
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND powershell -noprofile -executionpolicy Bypass -file "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/msbuild/applocal.ps1"
          -targetBinary $<TARGET_FILE:${TARGET_NAME}>
          -installedDir "${VCPKG_INSTALL_TRIPLETROOT}/bin"
          -OutVariable out
    )
  endif()
endfunction()


### VCPKG MAIN INIT
if (NOT MONDRADIKO_VCPKG_BOOTSTRAPPED)
    set(MONDRADIKO_VCPKG_BOOTSTRAPPED True)
    mondradiko_bootstrap_vcpkg()

    if (NOT USE_LAZY_VCPKG)
      mondradiko_execute_vcpkg(install mondradiko-deps)
    endif()

    if (BUILD_VULKAN_VALIDATION)
      mondradiko_execute_vcpkg(install vulkan-validation)
      message(">> To enable in-tree vulkan validation layers:\n\tset VK_LAYER_PATH=${VCPKG_INSTALL_TRIPLETROOT}/bin\n\t(before launching mondradiko-client.exe)")
    endif()
endif()
