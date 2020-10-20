#pragma once

#include <string>

#include "api_headers.hpp"
#include "assets/Asset.hpp"

class VulkanImage;
class VulkanInstance;

class TextureAsset : public Asset
{
public:
    TextureAsset(VulkanInstance*, aiTexture*);
    ~TextureAsset();

    VulkanImage* image = nullptr;
private:
    VulkanInstance* vulkanInstance;
};
