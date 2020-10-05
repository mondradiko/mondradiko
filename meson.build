project('mondradiko', 'cpp')

src = [
    'src/graphics/Renderer.cpp',
    'src/log/log.cpp',
    'src/network/NetworkInterface.cpp',
    'src/session/client_session.cpp',
    'src/xr/XrDisplay.cpp'
]

inc = include_directories(
    ['src']
)

dependencies = [
    dependency('openxr', version: '>=1.0')
]

executable(
    'mondradiko-client',
    ['src/client_main.cpp'] + src,
    include_directories: inc,
    dependencies: dependencies
)
