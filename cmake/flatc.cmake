# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

find_program(FLATC_COMMAND flatc)
if(NOT FLATC_COMMAND)
  message(FATAL_ERROR "flatc required - source maintained at https://github.com/google/flatbuffers")
endif()

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
		COMMAND ${FLATC_COMMAND} --scoped-enums -c -o ${OUTPUTDIR} ${FBS} 
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
