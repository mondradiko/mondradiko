// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuInstance;

class GpuShader {
 public:
  GpuShader(GpuInstance*, VkShaderStageFlagBits, const uint32_t*, size_t);
  ~GpuShader();

  VkPipelineShaderStageCreateInfo getStageCreateInfo() const;

 private:
  GpuInstance* gpu;

  VkShaderStageFlagBits stage_flags;

  VkShaderModule shader_module = VK_NULL_HANDLE;
};

}  // namespace core
}  // namespace mondradiko
