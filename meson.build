project('mondradiko', 'cpp')

src = [
    'src/assets/MeshAsset.cpp',
    'src/graphics/CameraDescriptorSet.cpp',
    'src/graphics/FrameData.cpp',
    'src/graphics/Renderer.cpp',
    'src/graphics/Viewport.cpp',
    'src/graphics/VulkanInstance.cpp',
    'src/graphics/pipelines/MeshPipeline.cpp',
    'src/graphics/shaders/MeshShader.cpp',
    'src/graphics/shaders/ShaderModule.cpp',
    'src/library_implementation.cpp',
    'src/log/log.cpp',
    'src/network/NetworkInterface.cpp',
    'src/session/client_session.cpp',
    'src/xr/XrDisplay.cpp',
    'src/xr/Session.cpp'
]

inc = include_directories(
    ['src',
     'lib']
)

dependencies = [
    dependency('openxr', version: '>=1.0'),
    dependency('vulkan'),
    dependency('shaderc'),
    dependency('assimp'),
    dependency('glm')
]

executable(
    'mondradiko-client',
    ['src/client_main.cpp'] + src,
    include_directories: inc,
    dependencies: dependencies
)
