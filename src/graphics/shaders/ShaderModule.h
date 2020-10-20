#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "api_headers.h"

class VulkanInstance;
using ShaderStages = std::vector<VkPipelineShaderStageCreateInfo>;

class ShaderModule
{
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
