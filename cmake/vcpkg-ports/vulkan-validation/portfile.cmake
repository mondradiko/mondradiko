vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/Vulkan-ValidationLayers
    REF v1.2.162
    HEAD_REF master
    SHA512 85eba0e1d07cafad212ef611fb1aef3113b6609361476cda7176a49110fe62d257b815f7c622d6ba959872785242e2231c7245242e7794ff5e60325d10f3061b
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS 
      -DBUILD_TESTS=OFF
      -DVULKAN_HEADERS_INSTALL_DIR=${CURRENT_INSTALLED_DIR}
      -DGLSLANG_INSTALL_DIR=${CURRENT_INSTALLED_DIR}/tools
      -DSPIRV_HEADERS_INSTALL_DIR=${CURRENT_INSTALLED_DIR}
      -DSPIRV_TOOLS_INSTALL_DIR=${CURRENT_INSTALLED_DIR}/tools/spirv-tools
)

vcpkg_install_cmake()

if (VCPKG_TARGET_IS_WINDOWS)
  file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/bin)
  file(RENAME ${CURRENT_PACKAGES_DIR}/lib/VkLayer_khronos_validation.dll ${CURRENT_PACKAGES_DIR}/bin/VkLayer_khronos_validation.dll)
  file(RENAME ${CURRENT_PACKAGES_DIR}/lib/VkLayer_khronos_validation.json ${CURRENT_PACKAGES_DIR}/bin/VkLayer_khronos_validation.json)
endif()

FILE(WRITE ${CURRENT_PACKAGES_DIR}/include/vulkan-valdation-placeholder.txt "// TODO: vcpkg complains if there is no include/ file included...")
file(INSTALL ${SOURCE_PATH}/LICENSE.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

#file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug)
