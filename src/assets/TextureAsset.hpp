#pragma once

#include <string>

#include "api_headers.hpp"
#include "assets/Asset.hpp"

class VulkanInstance;

class TextureAsset : public Asset
{
public:
    TextureAsset(std::string, VulkanInstance*, aiTexture*);
    ~TextureAsset();

    std::string textureName;
private:
    VulkanInstance* vulkanInstance;
};
