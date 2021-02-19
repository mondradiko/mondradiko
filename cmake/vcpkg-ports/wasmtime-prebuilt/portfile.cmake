if (VCPKG_TARGET_IS_WINDOWS)
	vcpkg_download_distfile(
		WASMTIME_ARCHIVE
		URLS https://github.com/bytecodealliance/wasmtime/releases/download/v0.21.0/wasmtime-v0.21.0-x86_64-windows-c-api.zip
		SHA512 0c45fafcc352914dc19958db84149ba3d24cad1252f07d7948d4b649a8090679fae8d8ee58683953f3271e8cc4c0de1483931dbb7f9384d493f159b0dcf2b416
		FILENAME wasmtime-v0.21.0-x86_64-windows-c-api.zip
	)
elseif (VCPKG_TARGET_IS_LINUX)
	vcpkg_download_distfile(
		WASMTIME_ARCHIVE
		URLS https://github.com/bytecodealliance/wasmtime/releases/download/v0.21.0/wasmtime-v0.21.0-x86_64-linux-c-api.tar.xz
		SHA512 35184a47c3c3db89ced2abe5900ae1a840d67f4f90a0ee79c5adcde11b84a47bdf5af78f9a929ce206a6687604d668ad2673f342a69ee11e74d7158a4b544de8
		FILENAME wasmtime-v0.21.0-x86_64-linux-c-api.tar.gz
	)
#elseif (VCPKG_TARGET_IS_OSX) 
#elseif (ANDROID)
else()
  vcpkg_fail_port_install(MESSAGE "only tested on Linux and Windows platforms")
endif ()

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH WASMTIME_EXTRACTED
	ARCHIVE ${WASMTIME_ARCHIVE}
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${WASMTIME_EXTRACTED})

vcpkg_configure_cmake(
    SOURCE_PATH ${WASMTIME_EXTRACTED}
    PREFER_NINJA
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug)

file(INSTALL ${WASMTIME_EXTRACTED}/include/
	DESTINATION ${CURRENT_PACKAGES_DIR}/include
)

if (VCPKG_TARGET_IS_WINDOWS)
  file(INSTALL ${WASMTIME_EXTRACTED}/lib/wasmtime.dll.lib DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
  file(INSTALL ${WASMTIME_EXTRACTED}/lib/wasmtime.dll DESTINATION ${CURRENT_PACKAGES_DIR}/bin)
else ()
  file(INSTALL ${WASMTIME_EXTRACTED}/lib/libwasmtime.a DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
  file(INSTALL ${WASMTIME_EXTRACTED}/lib/libwasmtime.so DESTINATION ${CURRENT_PACKAGES_DIR}/bin)
endif ()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
  file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

file(INSTALL ${WASMTIME_EXTRACTED}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/wasmtime-prebuilt/copyright)
