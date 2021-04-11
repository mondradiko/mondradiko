###############################################################################
# note: this preconfiguration needs to appear before first call to project()

# allow cmake to find locally-defined modules here under cmake/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# arrange for Release builds by default (TODO: maybe only for CI environments?)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release'"
      "(to override add -DCMAKE_BUILD_TYPE=Debug to override).")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (WIN32)
  # FIXME(humbletim): this policy override and setting is needed in order to
  #     link _Mondradiko_ Debug builds against the release versions of VCPKG
  #     dependencies and MSVCRT. The assumption here is that most people making
  #     Debug versions of mondradiko-client/server are interested in debugging
  #     mondradiko source code, not the underlying dependencies or MSVCRT.

  # dynamically link against non-debug version of msvcrtxxx.dll (/MD)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

  # enable VS Solution folders
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "predefined")
endif()

# VCPKG automation
include(mondradiko-vcpkg)
###############################################################################
