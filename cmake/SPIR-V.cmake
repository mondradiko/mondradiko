# Copyright 2019, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

find_program(GLSLANGVALIDATOR_COMMAND
	glslangValidator)
if(NOT GLSLANGVALIDATOR_COMMAND)
	message(FATAL_ERROR "glslangValidator required - source maintained at https://github.com/KhronosGroup/glslang")
endif()

#
# Generate a SPIR-V header file, with the given var name. Returns the header.
#
function(spirv_shader ret GLSL VAR)
	set(HEADER "${CMAKE_CURRENT_BINARY_DIR}/${GLSL}.h")
	set(GLSL "${CMAKE_CURRENT_SOURCE_DIR}/${GLSL}")

	add_custom_command(
		OUTPUT ${HEADER}
		COMMAND ${GLSLANGVALIDATOR_COMMAND} -V ${GLSL} --vn ${VAR} -o ${HEADER}
		DEPENDS ${GLSL})


	set(${ret} "${HEADER}" PARENT_SCOPE)
endfunction(spirv_shader)

#
# Generate SPIR-V header files from the arguments. Returns a list of headers.
#
function(spirv_shaders ret)

	foreach(GLSL ${ARGN})
		string(MAKE_C_IDENTIFIER ${GLSL} IDENTIFIER)
		spirv_shader(HEADER ${GLSL} ${IDENTIFIER})
		list(APPEND HEADERS ${HEADER})
	endforeach()

	set(${ret} "${HEADERS}" PARENT_SCOPE)
endfunction(spirv_shaders)
