/**
 * @file MeshShader.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains pipeline state for the MeshPipeline.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "graphics/shaders/MeshShader.h"

#include "log/log.h"

MeshShader::MeshShader(VulkanInstance* vulkanInstance)
    : vulkanInstance(vulkanInstance),
      vertShader(vulkanInstance, "MeshShader.vert", shaderc_vertex_shader),
      fragShader(vulkanInstance, "MeshShader.frag", shaderc_fragment_shader) {
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

MeshShader::~MeshShader() { log_dbg("Destroying mesh shader."); }

ShaderStages MeshShader::getStages() {
  return {vertShader.getStageCreateInfo(), fragShader.getStageCreateInfo()};
}
