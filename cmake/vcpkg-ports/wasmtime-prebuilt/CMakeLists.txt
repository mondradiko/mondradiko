cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(wasmtime LANGUAGES CXX)

include(GNUInstallDirs)

add_library(wasmtime INTERFACE)

target_link_directories(wasmtime INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>)

if (WIN32)
  target_link_libraries(wasmtime INTERFACE wasmtime.dll.lib)
else()
  target_link_libraries(wasmtime INTERFACE libwasmtime.so dl)
endif()

install(
    TARGETS wasmtime
    EXPORT wasmtime-config
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(
  EXPORT wasmtime-config
  NAMESPACE wasmtime::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/wasmtime
)

install(TARGETS wasmtime DESTINATION lib)
