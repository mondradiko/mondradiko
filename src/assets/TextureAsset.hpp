#pragma once

#include <string>

#include "api_headers.hpp"

class VulkanInstance;

class TextureAsset
{
public:
    TextureAsset(std::string, VulkanInstance*, aiTexture*);
    ~TextureAsset();

    std::string textureName;
private:
    VulkanInstance* vulkanInstance;
};
