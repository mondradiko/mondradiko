// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuShader.h"

#include "core/gpu/GpuInstance.h"
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
