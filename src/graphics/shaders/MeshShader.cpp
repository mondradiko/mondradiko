#include "graphics/shaders/MeshShader.hpp"

#include "log/log.hpp"

MeshShader::MeshShader(VulkanInstance* vulkanInstance)
 : vulkanInstance(vulkanInstance),
   vertShader(vulkanInstance, "MeshShader.vert", shaderc_vertex_shader),
   fragShader(vulkanInstance, "MeshShader.frag", shaderc_fragment_shader)
{
    log_dbg("Creating mesh shader.");

    vertShader.pushCustom(R"""(
layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
    )""");

    fragShader.pushCustom(R"""(
layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}
    )""");

    vertShader.compile();
    fragShader.compile();
}

MeshShader::~MeshShader()
{
    log_dbg("Destroying mesh shader.");
}

ShaderStages MeshShader::getStages()
{
    return{
        vertShader.getStageCreateInfo(),
        fragShader.getStageCreateInfo()
    };
}
