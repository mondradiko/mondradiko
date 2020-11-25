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
    'src/assets/MaterialAsset.cc',
    'src/assets/MeshAsset.cc',
    'src/assets/TextureAsset.cc',
    'src/components/Component.cc',
    'src/components/MeshRendererComponent.cc',
    'src/displays/OpenXrDisplay.cc',
    'src/displays/OpenXrViewport.cc',
    'src/displays/SdlDisplay.cc',
    'src/displays/SdlViewport.cc',
    'src/filesystem/AssimpIOStream.cc',
    'src/filesystem/AssimpIOSystem.cc',
    'src/filesystem/Filesystem.cc',
    'src/gpu/GpuBuffer.cc',
    'src/gpu/GpuImage.cc',
    'src/gpu/GpuInstance.cc',
    'src/gpu/GpuShader.cc',
    'src/renderer/MeshPipeline.cc',
    'src/renderer/Renderer.cc',
    'src/library_implementation.cc',
    'src/log/log.cc',
    'src/network/NetworkClient.cc',
    'src/network/NetworkShared.cc',
    'src/scene/Entity.cc',
    'src/scene/Scene.cc',
]

inc = include_directories(
    ['src',
     'lib',
     '.']
)

subdir('shaders')

dependencies = [
    dependency('openxr', version: '>=1.0'),
    dependency('vulkan'),
    dependency('shaderc'),
    dependency('assimp'),
    dependency('glm'),
    dependency('physfs'),
    dependency('sdl2'),

    shaders
]

executable(
    'mondradiko-client',
    ['src/client_main.cc'] + src,
    include_directories: inc,
    dependencies: dependencies
)
