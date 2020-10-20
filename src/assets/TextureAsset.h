#pragma once

#include <string>

#include "api_headers.h"
#include "assets/Asset.h"

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
