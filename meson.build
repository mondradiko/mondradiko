project('mondradiko', 'cpp')

src = [
    'src/log/log.cpp',
    'src/network/NetworkInterface.cpp',
    'src/session/client_session.cpp'
]

inc = include_directories(
    ['src']
)

executable(
    'mondradiko-client',
    ['src/client_main.cpp'] + src,
    include_directories: inc
)
