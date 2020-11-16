/**
 * @file ShaderModule.h
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

#ifndef SRC_GRAPHICS_SHADERMODULE_H_
#define SRC_GRAPHICS_SHADERMODULE_H_

#include <sstream>
#include <string>
#include <vector>

#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

using ShaderStages = std::vector<VkPipelineShaderStageCreateInfo>;

class ShaderModule {
 public:
  ShaderModule(GpuInstance*, std::string, shaderc_shader_kind);
  ~ShaderModule();

  void pushCustom(std::string);
  void pushFullscreenQuad();

  VkShaderModule compile();
  VkPipelineShaderStageCreateInfo getStageCreateInfo();

 private:
  GpuInstance* gpu;
  std::ostringstream glsl_source;
  std::string shader_name;
  shaderc_shader_kind shader_kind;
  VkShaderModule shader_module;
  bool compiled;
};

}  // namespace mondradiko

#endif  // SRC_GRAPHICS_SHADERMODULE_H_
