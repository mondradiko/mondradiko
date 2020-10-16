#include "assets/TextureAsset.hpp"

#include "log/log.hpp"

TextureAsset::TextureAsset(std::string textureName, VulkanInstance* vulkanInstance, aiTexture* texture)
 : textureName(textureName),
   vulkanInstance(vulkanInstance)
{
    log_dbg("Loading texture asset with name:");
    log_dbg(textureName.c_str());
}

TextureAsset::~TextureAsset()
{
    log_dbg("Destroying texture asset:");
    log_dbg(textureName.c_str());
}
