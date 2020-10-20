#include "graphics/shaders/MeshShader.h"

#include "log/log.h"

MeshShader::MeshShader(VulkanInstance* vulkanInstance)
 : vulkanInstance(vulkanInstance),
   vertShader(vulkanInstance, "MeshShader.vert", shaderc_vertex_shader),
   fragShader(vulkanInstance, "MeshShader.frag", shaderc_fragment_shader)
{
    log_dbg("Creating mesh shader.");

    vertShader.pushCustom(R"""(
layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = camera.projection * camera.view * vec4(vertPosition + vec3(0.0, 2.0, -2.0), 1.0);
    fragColor = vec3(vertTexCoord, 0.0);
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
