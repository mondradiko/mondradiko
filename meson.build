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
    'src/filesystem/AssimpIOStream.cc',
    'src/filesystem/AssimpIOSystem.cc',
    'src/filesystem/Filesystem.cc',
    'src/graphics/Renderer.cc',
    'src/graphics/Viewport.cc',
    'src/graphics/VulkanBuffer.cc',
    'src/graphics/VulkanImage.cc',
    'src/graphics/VulkanInstance.cc',
    'src/graphics/pipelines/MeshPipeline.cc',
    'src/graphics/shaders/MeshShader.cc',
    'src/graphics/shaders/ShaderModule.cc',
    'src/library_implementation.cc',
    'src/log/log.cc',
    'src/network/NetworkClient.cc',
    'src/network/NetworkShared.cc',
    'src/scene/Entity.cc',
    'src/scene/Scene.cc',
    'src/session/client_session.cc',
    'src/xr/PlayerSession.cc',
    'src/xr/XrDisplay.cc'
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
    ['src/client_main.cc'] + src,
    include_directories: inc,
    dependencies: dependencies
)
