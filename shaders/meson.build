glslangValidator = find_program('glslangValidator')

shader_srcs = [
    'mesh.frag',
    'mesh.vert'
]

shader_headers = []

foreach shader : shader_srcs
    header_name = shader + '.h'
    shader_headers += custom_target(
        header_name,
        command: [
            glslangValidator,
            '-V', '@INPUT@',
            '-o', '@OUTPUT@',
            '--vn', 'shaders_' + shader.underscorify()
        ],
        input: shader,
        output: header_name
    )
endforeach

shaders = declare_dependency(
    include_directories: include_directories('..'),
    sources: shader_headers
)
