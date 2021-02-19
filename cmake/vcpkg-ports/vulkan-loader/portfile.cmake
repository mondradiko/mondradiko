vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/Vulkan-Loader
    REF v1.2.162
    HEAD_REF master
    SHA512 89a99f282b73ec59dde30e4c8c09056b40416185a14ff9b7f7bd496754840e5ac764e7624f1f9a0f11792e786b00f19c3c9e55ccbb22a3c501d8ea9e815d84f9
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS -DVULKAN_HEADERS_INSTALL_DIR=${CURRENT_INSTALLED_DIR}
)

vcpkg_install_cmake()

FILE(WRITE ${CURRENT_PACKAGES_DIR}/include/vulkan-loader-placeholder.txt "// TODO: vcpkg complains if there is no include/ file included...")
file(INSTALL ${SOURCE_PATH}/LICENSE.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)


#file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug)
