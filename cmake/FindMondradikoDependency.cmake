
# find and register a package as _opts_ALIAS preferring pkg-config if available,
#   and if not found optionally falling back to vcpkg_installed/
# example:
#   find_mondradiko_dependency(
#     mondradiko::xxhash
#     PKG "XXHASH" PKG_MODULES libxxhash
#     FALLBACK VCPKG "xxHash" VCPKG_MODULES xxHash::xxhash
#   )
function(find_mondradiko_dependency _opts_ALIAS)
    set(options FALLBACK)
    set(oneValueArgs VCPKG PKG)
    set(multiValueArgs PKG_MODULES VCPKG_MODULES)
    cmake_parse_arguments(_opts "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})  
    if (WIN32 OR NOT _opts_PKG OR USE_VCPKG)
      find_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${_opts_VCPKG_MODULES})
    else()
      set(_opts_args "")
      if (NOT _opts_FALLBACK)
        list(APPEND _opts_args REQUIRED)
      endif()
      list(APPEND _opts_args ${_opts_PKG_MODULES})
      find_package_pkgconfig(${_opts_ALIAS} ${_opts_PKG} ${_opts_args})
      if (NOT TARGET ${_opts_ALIAS})
        if (_opts_FALLBACK)
          message(STATUS "(pkg-config[${_opts_PKG}]): fallilng back to VCPKG...")
          unset(${_opts_PKG}_FOUND CACHE)
          find_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${_opts_VCPKG_MODULES})
        endif()
      endif()
      # message(STATUS "calling pkg_check_modules ${_opts_PKG} ${_opts_REQUIRED}")
    endif()
endfunction()

if (WIN32)
  set(_find_triplet "x64-windows")
endif()
if (UNIX)
  set(_find_triplet "x64-linux")
endif()

# enable find_program() to locate utilities installed to vcpkg_installed/tools/
list(APPEND CMAKE_PREFIX_PATH 
  "${CMAKE_BINARY_DIR}/vcpkg_installed/${_find_triplet}/tools"
)

if (NOT WIN32)
  find_package(PkgConfig REQUIRED)
endif()

# find_package_vcpkg(<alias> <vcpkg-name> <vcpkg-modules>)
# ... finds a previous installed VCPKG package and registers as _opts_ALIAS
# eg: find_package_vcpkg(mondradiko::sdl2 sdl2 SDL2::SDL2)
function(find_package_vcpkg _opts_ALIAS _opts_VCPKG)
  find_package(${_opts_VCPKG} CONFIG REQUIRED HINTS ${CMAKE_BINARY_DIR}/vcpkg_installed/${_find_triplet}/share/${opts_VCPKG}) # NAMES ${opts_VCPKG} )
  if (NOT ${_opts_VCPKG}_FOUND)
    message(FATAL_ERROR "(vcpkg[${_opts_VCPKG}]): NOT FOUND ${_opts_ALIAS} ${CMAKE_BINARY_DIR}/vcpkg_installed/${_find_triplet}/share/${opts_VCPKG}")
  else()
    if (NOT TARGET ${_opts_ALIAS})
      message(STATUS "@vcpkg: ${_opts_ALIAS}")
      add_library(${_opts_ALIAS} INTERFACE IMPORTED)
      set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_LINK_LIBRARIES ${ARGN})
      set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/vcpkg_installed/${_find_triplet}/include)
    else()
      message(STATUS "(vcpkg): ${_opts_ALIAS} already exists... not redefining")
    endif()
  endif()
endfunction()

# find_package_pkgconfig(<alias> <pkg-name> <pkg-modules>)
# ... finds a previously installed pkg-config mod and registers as _opts_ALIAS
# eg: find_package_pkgconfig(mondradiko::sdl2 SDL2 sdl2)
function(find_package_pkgconfig _opts_ALIAS _opts_PKG)
  # message(STATUS "find_package_pkgconfig(${_opts_ALIAS} ${_opts_PKG} ${ARGN})")
  #list(PREPEND ARGN QUIET)
  pkg_check_modules(${_opts_PKG} ${ARGN})
  
  if (_pkgconfig_error)
    message(STATUS "(pkg-config[${_opts_PKG}]): NOT FOUND ${_opts_ALIAS} ${_pkgconfig_error}")
  else()
    # message(STATUS "(pkg-config): FOUND ${_opts_ALIAS} ${PKG_CONFIG_FOUND}")
    if (NOT TARGET ${_opts_ALIAS})
      message(STATUS "@pkg-config[${_opts_PKG}]: ${_opts_ALIAS} include:${${_opts_PKG}_INCLUDE_DIRS} libs: ${${_opts_PKG}_LIBRARIES} link: ${${_opts_PKG}_LINK_LIBRARIES}")
      add_library(${_opts_ALIAS} INTERFACE IMPORTED)
      if(${_opts_PKG}_INCLUDE_DIRS)
        set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${_opts_PKG}_INCLUDE_DIRS}")
      endif()
      if(${_opts_PKG}_LINK_LIBRARIES)
        set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_LINK_LIBRARIES "${${_opts_PKG}_LINK_LIBRARIES}")
      endif()
      if(${_opts_PKG}_LDFLAGS_OTHER)
        set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_LINK_OPTIONS "${${_opts_PKG}_LDFLAGS_OTHER}")
      endif()
      if(${_opts_PKG}_CFLAGS_OTHER)
        set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_COMPILE_OPTIONS "${${_opts_PKG}_CFLAGS_OTHER}")
      endif()    
    else()
      message(STATUS "(pkg-config[${_opts_PKG}]): ${_opts_ALIAS} already exists... not redefining")
    endif()
  endif()
endfunction()

# VCPKG_APPLOCAL_DEPS works fine to ensure runtime DLLs get copied for Release
# builds, but not in the case of compiling the project in Debug mode while
# still linking against VCPKG Release dependencies.
#
# This custom command replicates the same effect so Visual Studio users
# can build an application .exe in Debug mode and simply hit F5 to start
# debugging it (without first having to manually shuffle DLLs around).

function(mondradiko_instrument_exe_runtime_dlls TARGET_NAME)
  if (USE_VCPKG AND WIN32)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND powershell -noprofile -executionpolicy Bypass -file "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/msbuild/applocal.ps1"
          -targetBinary $<TARGET_FILE:${TARGET_NAME}>
          -installedDir "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin"
          -OutVariable out
    )
  endif()
endfunction()
