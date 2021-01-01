# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Python3 REQUIRED)
set(GENERATE_BINDING_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_binding.py")
set(GENERATE_LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_script_linker.py")

set(BINDING_SRC
  components/Transform.toml
)

#
# Generate a C++ binding class. Returns the generated header.
#
function(cpp_binding ret BINDING)
  get_filename_component(BASEDIR ${BINDING} DIRECTORY)
  get_filename_component(BASENAME ${BINDING} NAME_WE)
  set(OUTPUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${BASEDIR}")
  set(HEADER "${OUTPUTDIR}/${BASENAME}.h")
  set(BINDING "${CMAKE_CURRENT_SOURCE_DIR}/${BINDING}")

  add_custom_command(
    OUTPUT ${HEADER}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_BINDING_SCRIPT} ${HEADER} ${BINDING}
    DEPENDS ${BINDING} ${GENERATE_BINDING_SCRIPT})

  set(${ret} "${HEADER}" PARENT_SCOPE)
endfunction(cpp_binding)

#
# Generates C++ bindings. Returns the script linker source file.
#
function(cpp_bindings ret BINDINGS)

  foreach(BINDING ${BINDINGS})
    cpp_binding(HEADER ${BINDING})
    list(APPEND BINDING_HEADERS ${HEADER})
  endforeach()

  # Generate source file to link Wasm imported functions
  set(SCRIPT_LINKER_SRC "${CMAKE_CURRENT_BINARY_DIR}/script_linker.cc")
  add_custom_command(
    OUTPUT ${SCRIPT_LINKER_SRC}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_LINK_SCRIPT} ${SCRIPT_LINKER_SRC} ${BINDINGS}
    DEPENDS ${GENERATE_LINK_SCRIPT} ${BINDING_HEADERS}
  )

  set(${ret} ${SCRIPT_LINKER_SRC} PARENT_SCOPE)
endfunction(cpp_bindings)
