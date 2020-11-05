/**
 * @file MeshShader.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains pipeline state for the MeshPipeline.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "graphics/shaders/MeshShader.h"

#include "log/log.h"

namespace mondradiko {

MeshShader::MeshShader(VulkanInstance* vulkanInstance)
    : vulkanInstance(vulkanInstance),
      vertShader(vulkanInstance, "MeshShader.vert", shaderc_vertex_shader),
      fragShader(vulkanInstance, "MeshShader.frag", shaderc_fragment_shader) {
  log_dbg("Creating mesh shader.");

  vertShader.pushCustom(R"""(
// TODO(marceline-cramer) Overwrite this with more viewports when they are added
layout(constant_id = 0) const int CAMERA_NUM = 2;

layout(push_constant) uniform push_constants_t {
  uint view_index;
} push_constants;

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} cameras[CAMERA_NUM];

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
  uint view_index = push_constants.view_index;
  gl_Position = cameras[view_index].projection * cameras[view_index].view * vec4(vertPosition + vec3(0.0, 2.0, -2.0), 1.0);

  fragColor = vec3(vertTexCoord, 0.0);
  fragTexCoord = vertTexCoord;
}
    )""");

  fragShader.pushCustom(R"""(
layout(constant_id = 1) const int TEXTURE_NUM = 128;

layout(set = 0, binding = 1) uniform sampler2D textures[TEXTURE_NUM];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = texture(textures[0], fragTexCoord);  // * vec4(fragColor, 1.0);
}
    )""");

  vertShader.compile();
  fragShader.compile();
}

MeshShader::~MeshShader() { log_dbg("Destroying mesh shader."); }

ShaderStages MeshShader::getStages() {
  return {vertShader.getStageCreateInfo(), fragShader.getStageCreateInfo()};
}

}  // namespace mondradiko
