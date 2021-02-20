# header-only library + API registry
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/Vulkan-Headers
    REF v1.2.162
    SHA512 7b33f6ef32f1ef34d3b37ad38b077a3a2ebcef62612371afa1b6f4910815bbab8418a837c49b3872b57d2e5012c15623521121f5d62836e46d6f20773841d7b0
    HEAD_REF master
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)

# note: building and installing Vulkan-Headers is necessary to generate the
#  Vulkan API Registry files (which live in share/vulkan/registry/*)
vcpkg_install_cmake()

# Handle copyright
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
