#include "assets/TextureAsset.hpp"

#include "graphics/VulkanImage.hpp"
#include "log/log.hpp"

TextureAsset::TextureAsset(VulkanInstance* vulkanInstance, aiTexture* texture)
 : vulkanInstance(vulkanInstance)
{
    image = new VulkanImage(vulkanInstance,
        VK_FORMAT_R8G8B8A8_UNORM,
        texture->mWidth, texture->mHeight,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    );
}

TextureAsset::~TextureAsset()
{
    if(image) delete image;
}
