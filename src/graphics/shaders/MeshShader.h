#pragma once

#include "graphics/shaders/ShaderModule.h"

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
