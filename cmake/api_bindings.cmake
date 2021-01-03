# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Python3 REQUIRED)
set(GENERATE_BINDING_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_binding.py")

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
