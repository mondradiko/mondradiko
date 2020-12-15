/**
 * @file GpuShader.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates a shader module from SPIR-V bytecode.
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/gpu/GpuInstance.h"

namespace mondradiko {

class GpuShader {
 public:
  GpuShader(GpuInstance*, VkShaderStageFlagBits, const uint32_t*, size_t);
  ~GpuShader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo();
 private:
  GpuInstance* gpu;

  VkShaderStageFlagBits stage_flags;

  VkShaderModule shader_module = VK_NULL_HANDLE;
};

}  // namespace mondradiko
