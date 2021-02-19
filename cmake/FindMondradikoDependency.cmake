### Mondradiko CMake dependency helpers

### Unified dependency locator (pkg-config + vcpkg)
#
# this helper function finds and registers a package as _opts_ALIAS:
#   when -DUSE_PKGCONFIG=ON it will first check pkg-config for a match and then
#     automatically fall back to VCPKG
#
# TODO(humbletim) maybe also support "git submodule" overrides too so that all
#   three strategies can be specified together (submodule, pkg-config, vcpkg)
#
# find_mondradiko_dependency(
#   <internal alias>
#   PKG <pkg-name> PKG_MODULES <pkg-config module spec(s)>
#   FALLBACK VCPKG <vcpkg-name> [VCPKG_INSTALL <vcpkg install name>] VCPKG_MODULES <vcpkg module names>
# )
#
# example:
#   find_mondradiko_dependency(
#     mondradiko::xxhash
#     PKG "XXHASH" PKG_MODULES libxxhash
#     FALLBACK VCPKG "xxHash" VCPKG_MODULES xxHash::xxhash
#   )
# ... then to reference the dep's includes + libs requires just one line:
# target_link_libraries(<target_name> PUBLIC mondradiko::xxhash)
#

function(_find_or_install_package_vcpkg _opts_ALIAS _opts_VCPKG _opts_VCPKG_INSTALL)
  if (NOT TARGET ${_opts_ALIAS})
    find_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${ARGN})
    if (NOT TARGET ${_opts_ALIAS})
      message(STATUS "(vcpkg[${_opts_VCPKG}]): attempting to autoinstall... \"${_opts_VCPKG}\" as \"${_opts_VCPKG_INSTALL}\"")
      mondradiko_execute_vcpkg(install ${_opts_VCPKG_INSTALL})
      find_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} REQUIRED ${ARGN})
      if (NOT TARGET ${_opts_ALIAS})
        message(FATAL_ERROR "unable to find/install package via VCPKG: ${_opts_VCPKG}")
      endif()
    endif()
  endif()
endfunction()

function(find_mondradiko_dependency _opts_ALIAS)
    set(options "") #FALLBACK)
    set(oneValueArgs VCPKG VCPKG_INSTALL PKG)
    set(multiValueArgs PKG_MODULES VCPKG_MODULES)
    cmake_parse_arguments(_opts "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})  
    if (NOT _opts_VCPKG_INSTALL)
      set(_opts_VCPKG_INSTALL ${_opts_VCPKG})
    endif()
    if (TARGET ${_opts_ALIAS})
      message(STATUS "[find_mondradiko_dependency target already defined: ${_opts_ALIAS}]")
      return()
    endif()
    if (NOT _opts_PKG OR WIN32 OR NOT USE_PKGCONFIG)
      _find_or_install_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${_opts_VCPKG_INSTALL} ${_opts_VCPKG_MODULES})
      return()
    endif()
    if (ON)
      set(_opts_args QUIET)
      list(APPEND _opts_args ${_opts_PKG_MODULES})
      find_package_pkgconfig(${_opts_ALIAS} ${_opts_PKG} QUIET ${_opts_args})
      if (TARGET ${_opts_ALIAS})
        message(STATUS "target created from pkg-config: ${_opts_ALIAS}")
      endif()

      if (NOT TARGET ${_opts_ALIAS})
        if (_opts_VCPKG)
          #message(STATUS "(pkg-config[${_opts_PKG}]): falling back to VCPKG... \"${_opts_VCPKG}\"")
          unset(${_opts_PKG}_FOUND CACHE)
          _find_or_install_package_vcpkg(${_opts_ALIAS} ${_opts_VCPKG} ${_opts_VCPKG_INSTALL} ${_opts_VCPKG_MODULES})
        else()
          message(FATAL_ERROR "unable to find/install package VCPKG: ${_opts_VCPKG}")
        endif()
      endif()
      # message(STATUS "calling pkg_check_modules ${_opts_PKG} ${_opts_REQUIRED}")
    endif()
    if (NOT TARGET ${_opts_ALIAS})
      message(FATAL_ERROR "ERROR LOCATING DEPENDENCY ${_opts_ALIAS} ${_opts_PKG} ${_opts_VCPKG}")
    endif()
endfunction()


### VCPKG LOOKUP

# find_package_vcpkg(<alias> [REQUIRED] <vcpkg-name> <vcpkg-modules>)
# ... finds a previous installed VCPKG package and registers as _opts_ALIAS
# eg: find_package_vcpkg(mondradiko::sdl2 sdl2 SDL2::SDL2)
function(find_package_vcpkg _opts_ALIAS _opts_VCPKG)
  cmake_parse_arguments(_opts "REQUIRED;QUIET" "" "" ${ARGN})
  #message(STATUS "find_package_vcpkg -- unparsed: ${_opts_UNPARSED_ARGUMENTS} REQUIRED=${_opts_REQUIRED}")
  set(_ARGS "")
  if (_opts_REQUIRED)
    list(APPEND _ARGS REQUIRED)
  endif()
  if (_opts_QUIET)
    list(APPEND _ARGS QUIET)
  endif()
  find_package(${_opts_VCPKG} CONFIG ${_ARGS} HINTS ${VCPKG_INSTALL_TRIPLETROOT}/share/${opts_VCPKG}) # NAMES ${opts_VCPKG} )
  if (NOT ${_opts_VCPKG}_FOUND)
    if (_REQUIRED)
      message(FATAL_ERROR "(vcpkg[${_opts_VCPKG}]): NOT FOUND ${_opts_ALIAS}")
    else()
      message(STATUS "(vcpkg[${_opts_VCPKG}]): NOT FOUND ${_opts_ALIAS}")
    endif()
  else()
    if (NOT TARGET ${_opts_ALIAS})
      message(STATUS "@vcpkg: ${_opts_ALIAS}")
      add_library(${_opts_ALIAS} INTERFACE IMPORTED)
      set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_LINK_LIBRARIES "${_opts_UNPARSED_ARGUMENTS}")
      set_property(TARGET ${_opts_ALIAS} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_INSTALL_TRIPLETROOT}/include")
    else()
      message(STATUS "(vcpkg): ${_opts_ALIAS} already exists... not redefining")
    endif()
  endif()
endfunction()


### PKG-CONFIG LOOKUP

if (USE_PKGCONFIG AND NOT WIN32)
  find_package(PkgConfig REQUIRED)
endif()

# find_package_pkgconfig(<alias> <pkg-name> <pkg-modules>)
# ... finds a previously installed pkg-config mod and registers as _opts_ALIAS
# eg: find_package_pkgconfig(mondradiko::sdl2 SDL2 sdl2)
function(find_package_pkgconfig _opts_ALIAS _opts_PKG)
  # message(STATUS "find_package_pkgconfig(${_opts_ALIAS} ${_opts_PKG} ${ARGN})")
  #list(PREPEND ARGN QUIET)
  pkg_check_modules(${_opts_PKG} QUIET ${ARGN})
  
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


### MISC

# VCPKG_APPLOCAL_DEPS works fine to ensure runtime DLLs get copied for Release
# builds, but not in the case of compiling the project in Debug mode while
# still linking against VCPKG Release dependencies.
#
# This custom command replicates the same effect so Visual Studio users
# can build an application .exe in Debug mode and simply hit F5 to start
# debugging it (without first having to manually shuffle DLLs around).
function(mondradiko_instrument_exe_runtime_dlls TARGET_NAME)
  if (WIN32)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND powershell -noprofile -executionpolicy Bypass -file "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/msbuild/applocal.ps1"
          -targetBinary $<TARGET_FILE:${TARGET_NAME}>
          -installedDir "${VCPKG_INSTALL_TRIPLETROOT}/bin"
          -OutVariable out
    )
  endif()
endfunction()
