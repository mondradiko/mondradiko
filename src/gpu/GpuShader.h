/**
 * @file GpuShader.h
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

#ifndef SRC_GPU_GPUSHADER_H_
#define SRC_GPU_GPUSHADER_H_

#include "gpu/GpuInstance.h"

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

#endif  // SRC_GPU_GPUSHADER_H_
