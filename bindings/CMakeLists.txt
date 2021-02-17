# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

set(Python3_EXECUTABLE $ENV{Python3_EXECUTABLE})
if (Python3_EXECUTABLE)
  message(STATUS "Using Python3_EXECUTABLE = ${Python3_EXECUTABLE}")
else()
  find_package(Python3 3.6 REQUIRED)
endif()

set(GENERATE_BINDING_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generate_binding.py")

#
# Generate API bindings for a component. Returns the generated linker.
#
function(api_binding ret BINDING)
  get_filename_component(BASEDIR ${BINDING} DIRECTORY)
  get_filename_component(BASENAME ${BINDING} NAME_WE)
  set(OUTPUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${BASEDIR}")
  set(LINKER "${OUTPUTDIR}/${BASENAME}_linker.cc")
  set(CPP_BINDING "${OUTPUTDIR}/${BASENAME}.h")
  set(BINDING "${CMAKE_CURRENT_SOURCE_DIR}/${BINDING}")

  add_custom_command(
    OUTPUT ${LINKER}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_BINDING_SCRIPT} ${BINDING} ${LINKER} ${CPP_BINDING}
    DEPENDS ${BINDING} ${GENERATE_BINDING_SCRIPT})

  set(${ret} "${LINKER}" PARENT_SCOPE)
endfunction(api_binding)

#
# Generates API bindings. Returns the script linker source files.
#
function(api_bindings ret BINDINGS)

  foreach(BINDING ${BINDINGS})
    api_binding(HEADER ${BINDING})
    list(APPEND SCRIPT_LINKER_SRC ${HEADER})
  endforeach()

  set(${ret} ${SCRIPT_LINKER_SRC} PARENT_SCOPE)
endfunction(api_bindings)

# Create components/ binary directory
add_subdirectory(components)

api_bindings(SCRIPT_LINKERS
  components/Transform.toml
)

add_library(mondradiko-api-components OBJECT ${SCRIPT_LINKERS})
# TODO(marceline-cramer): Don't expose third-party dependencies to the public interface
add_dependencies(mondradiko-api-components mondradiko-types)
target_link_libraries(mondradiko-api-components mondradiko-lib)
