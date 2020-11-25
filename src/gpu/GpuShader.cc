/**
 * @file GpuShader.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates a shader module from SPIR-V bytecode.
 * @date 2020-11-25
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

#include "gpu/GpuShader.h"
#include "log/log.h"

namespace mondradiko {

GpuShader::GpuShader(GpuInstance* gpu, VkShaderStageFlagBits stage_flags,
                     const uint32_t* spirv_data, size_t spirv_size)
    : gpu(gpu), stage_flags(stage_flags) {
  VkShaderModuleCreateInfo module_info{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = spirv_size,
      .pCode = spirv_data};

  if (vkCreateShaderModule(gpu->device, &module_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    log_ftl("Failed to create shader module.");
  }
}

GpuShader::~GpuShader() {
  if (shader_module != VK_NULL_HANDLE)
    vkDestroyShaderModule(gpu->device, shader_module, nullptr);
}

VkPipelineShaderStageCreateInfo GpuShader::getStageCreateInfo() {
  return VkPipelineShaderStageCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = stage_flags,
      .module = shader_module,
      .pName = "main"};
}

}  // namespace mondradiko
