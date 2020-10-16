project('mondradiko', 'cpp')

majorVersion = 0
minorVersion = 0
patchVersion = 1

versionTemplate = 'v@0@.@1@.@2@'
versionString = versionTemplate.format(majorVersion, minorVersion, patchVersion)

configData = configuration_data()

configData.set('name', 'Mondradiko')
configData.set('version', versionString)
configData.set('majorVersion', majorVersion)
configData.set('minorVersion', minorVersion)
configData.set('patchVersion', patchVersion)

configure_file(
    input: 'src/build_config.h.in',
    output: 'build_config.h',
    configuration: configData
)

src = [
    'src/assets/MeshAsset.cpp',
    'src/assets/TextureAsset.cpp',
    'src/filesystem/AssimpIOStream.cpp',
    'src/filesystem/AssimpIOSystem.cpp',
    'src/filesystem/Filesystem.cpp',
    'src/graphics/CameraDescriptorSet.cpp',
    'src/graphics/FrameData.cpp',
    'src/graphics/Renderer.cpp',
    'src/graphics/Viewport.cpp',
    'src/graphics/VulkanBuffer.cpp',
    'src/graphics/VulkanInstance.cpp',
    'src/graphics/pipelines/MeshPipeline.cpp',
    'src/graphics/shaders/MeshShader.cpp',
    'src/graphics/shaders/ShaderModule.cpp',
    'src/library_implementation.cpp',
    'src/log/log.cpp',
    'src/network/ClientInterface.cpp',
    'src/network/NetworkInterface.cpp',
    'src/scene/Scene.cpp',
    'src/session/client_session.cpp',
    'src/xr/PlayerSession.cpp',
    'src/xr/XrDisplay.cpp'
]

inc = include_directories(
    ['src',
     'lib',
     '.']
)

dependencies = [
    dependency('openxr', version: '>=1.0'),
    dependency('vulkan'),
    dependency('shaderc'),
    dependency('assimp'),
    dependency('glm'),
    dependency('physfs')
]

executable(
    'mondradiko-client',
    ['src/client_main.cpp'] + src,
    include_directories: inc,
    dependencies: dependencies
)
