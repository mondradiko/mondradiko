# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

include(FindMondradikoDependency)

find_program(FLATC_COMMAND flatc PATH_SUFFIXES flatbuffers)
if(NOT FLATC_COMMAND)
  message(STATUS "CMAKE_PROGRAM_PATH == ${CMAKE_PROGRAM_PATH}")
  message(FATAL_ERROR "flatc required - source maintained at https://github.com/google/flatbuffers")
endif()

message(STATUS ">>> FLATC_COMMAND ${FLATC_COMMAND}")

#
# Generate a flatbuffers header file. Returns the header.
#
function(flatc_schema ret FBS)
  get_filename_component(BASEDIR ${FBS} DIRECTORY)
  get_filename_component(BASENAME ${FBS} NAME_WE)
  set(OUTPUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${BASEDIR}")
  set(HEADER "${OUTPUTDIR}/${BASENAME}_generated.h")
  set(FBS "${CMAKE_CURRENT_SOURCE_DIR}/${FBS}")

  add_custom_command(
		OUTPUT ${HEADER}
		COMMAND ${FLATC_COMMAND} --gen-object-api --gen-mutable --scoped-enums -c -o ${OUTPUTDIR} ${FBS}
		DEPENDS ${FBS})

	set(${ret} "${HEADER}" PARENT_SCOPE)
endfunction(flatc_schema)

#
# Generate flatbuffers header files. Returns a list of headers.
#
function(flatc_schemas ret)

	foreach(FBS ${ARGN})
		flatc_schema(HEADER ${FBS})
		list(APPEND HEADERS ${HEADER})
	endforeach()

	set(${ret} "${HEADERS}" PARENT_SCOPE)
endfunction(flatc_schemas)
