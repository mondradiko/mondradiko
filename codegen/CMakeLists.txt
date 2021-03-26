# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

# CI support for specifying an exact Python3_EXECUTABLE via environment var
set(Python3_EXECUTABLE $ENV{Python3_EXECUTABLE})
if (Python3_EXECUTABLE)
  message(STATUS "Using Python3_EXECUTABLE = ${Python3_EXECUTABLE}")
else()
  find_package(Python3 3.6 REQUIRED)
endif()

set(GENERATE_CLASS_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generate_class.py")

# Regenerate class wrappers when generator scripts are modified
set(CLASSDEF_GENERATOR_SCRIPTS
  "${CMAKE_CURRENT_SOURCE_DIR}/as_binding.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/codegen.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/wasm_linker.py"
)

#
# Generate wrapper code for a class definition. Returns the generated file.
#
function(wrap_class ret GENERATOR EXTENSION CLASSDEF)
  get_filename_component(BASEDIR ${CLASSDEF} DIRECTORY)
  get_filename_component(BASENAME ${CLASSDEF} NAME_WE)
  set(OUTPUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${BASEDIR}")
  set(OUTFILE "${OUTPUTDIR}/${BASENAME}${EXTENSION}")
  set(CLASSDEF "${CMAKE_CURRENT_SOURCE_DIR}/${CLASSDEF}")

  add_custom_command(
    OUTPUT ${OUTFILE}
    COMMAND ${Python3_EXECUTABLE} ${GENERATE_CLASS_SCRIPT} ${GENERATOR} ${CLASSDEF} ${OUTFILE}
  DEPENDS ${CLASSDEF} ${GENERATE_CLASS_SCRIPT} ${CLASSDEF_GENERATOR_SCRIPTS})

  set(${ret} "${OUTFILE}" PARENT_SCOPE)
endfunction(wrap_class)

#
# Generates multiple class definition wrappers. Returns the generated files.
#
function(wrap_classes ret GENERATOR CLASSDEFS)

  if(${GENERATOR} STREQUAL "wasm-linker")
    set(EXTENSION "_linker.cc")
  elseif(${GENERATOR} STREQUAL "as-binding")
    set(EXTENSION ".d.ts")
  else()
    message(FATAL_ERROR "Unrecognized generator " ${GENERATOR})
  endif()

  foreach(CLASSDEF ${CLASSDEFS})
    wrap_class(OUTFILE ${GENERATOR} ${EXTENSION} ${CLASSDEF})
    list(APPEND OUTFILES ${OUTFILE})
  endforeach()

  set(${ret} ${OUTFILES} PARENT_SCOPE)
endfunction(wrap_classes)

# Create components/ binary directory
add_subdirectory(components)

set(COMPONENT_CLASSDEFS
  components/Transform.toml
)

wrap_classes(SCRIPT_LINKERS "wasm-linker" ${COMPONENT_CLASSDEFS})
wrap_classes(AS_BINDINGS "as-binding" ${COMPONENT_CLASSDEFS})

add_library(mondradiko-as-bindings INTERFACE ${AS_BINDINGS})

add_library(mondradiko-wasm-linker OBJECT ${SCRIPT_LINKERS})

# TODO(marceline-cramer): Don't expose third-party dependencies to the public interface
add_dependencies(mondradiko-wasm-linker mondradiko-types)
# This is needed to include the wasmtime headers
target_link_libraries(mondradiko-wasm-linker mondradiko-lib)
