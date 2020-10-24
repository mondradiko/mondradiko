/**
 * @file ShaderModule.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class to compile shader code and provide creation info.
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

#ifndef SRC_GRAPHICS_SHADERS_SHADERMODULE_H_
#define SRC_GRAPHICS_SHADERS_SHADERMODULE_H_

#include <sstream>
#include <string>
#include <vector>

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

using ShaderStages = std::vector<VkPipelineShaderStageCreateInfo>;

class ShaderModule {
 public:
  ShaderModule(VulkanInstance*, std::string, shaderc_shader_kind);
  ~ShaderModule();

  void pushCustom(std::string);
  void pushFullscreenQuad();

  VkShaderModule compile();
  VkPipelineShaderStageCreateInfo getStageCreateInfo();

 private:
  VulkanInstance* vki;
  std::ostringstream glslSource;
  std::string shaderName;
  shaderc_shader_kind shaderKind;
  VkShaderModule shaderModule;
  bool compiled;
};

#endif  // SRC_GRAPHICS_SHADERS_SHADERMODULE_H_
