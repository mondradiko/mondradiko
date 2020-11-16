/**
 * @file ShaderModule.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class to compile shader code and provide creation info.
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

#include "graphics/ShaderModule.h"

#include "log/log.h"

namespace mondradiko {

ShaderModule::ShaderModule(GpuInstance* gpu, std::string shader_name,
                           shaderc_shader_kind shader_kind)
    : gpu(gpu),
      shader_name(shader_name),
      shader_kind(shader_kind),
      compiled(false) {
  glsl_source << "#version 450" << std::endl;
  glsl_source << "#extension GL_ARB_separate_shader_objects : enable"
              << std::endl;
}

ShaderModule::~ShaderModule() {
  if (compiled) {
    vkDestroyShaderModule(gpu->device, shader_module, nullptr);
  }
}

void ShaderModule::pushCustom(std::string code) {
  glsl_source << code << std::endl;
}

void ShaderModule::pushFullscreenQuad() {
  glsl_source << R"""(
layout(location = 0) out vec2 fragTexCoord;

vec2 positions[4] = vec2[](
    vec2( 1.0, -1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

vec2 texCoords[4] = vec2[](
    vec2(1.0, 0.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragTexCoord = texCoords[gl_VertexIndex];
}
    )""" << std::endl;
}

VkShaderModule ShaderModule::compile() {
  if (compiled) return shader_module;

  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  std::string complete_source = glsl_source.str();
  auto spirv_module = compiler.CompileGlslToSpv(complete_source, shader_kind,
                                                shader_name.data(), options);

  if (spirv_module.GetCompilationStatus() !=
      shaderc_compilation_status_success) {
    std::ostringstream errorMessage;
    errorMessage << "Shader " << shader_name;
    errorMessage << " compilation failed with ";
    errorMessage << spirv_module.GetErrorMessage();
    log_ftl(errorMessage.str().c_str());
  }

  std::vector<uint32_t> spirv = {spirv_module.cbegin(), spirv_module.cend()};

  VkShaderModuleCreateInfo module_info{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = spirv.size() * sizeof(uint32_t),
      .pCode = spirv.data()};

  if (vkCreateShaderModule(gpu->device, &module_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    log_ftl("Failed to create shader module");
  }

  compiled = true;
  return shader_module;
}

VkPipelineShaderStageCreateInfo ShaderModule::getStageCreateInfo() {
  if (shader_kind == shaderc_vertex_shader) {
    return {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shader_module,
            .pName = "main"};
  } else if (shader_kind == shaderc_fragment_shader) {
    return {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader_module,
            .pName = "main"};
  } else {
    log_ftl("Unrecognized shader type");
    return {};
  }
}

}  // namespace mondradiko
