#pragma once

#include "graphics/shaders/ShaderModule.hpp"

class MeshShader
{
public:
    MeshShader(VulkanInstance*);
    ~MeshShader();

    ShaderStages getStages();
private:
    VulkanInstance* vulkanInstance;

    ShaderModule vertShader;
    ShaderModule fragShader;
};
