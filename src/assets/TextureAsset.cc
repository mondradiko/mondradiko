/**
 * @file TextureAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains a texture, intended for mapping to meshes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "assets/TextureAsset.h"

#include "graphics/VulkanImage.h"
#include "log/log.h"

namespace mondradiko {

TextureAsset::TextureAsset(VulkanInstance* vulkanInstance, aiTexture* texture)
    : vulkanInstance(vulkanInstance) {
  // If the height of the texture is 0,
  // that means that it's stored in a compressed format
  // like PNG, JPG, etc., and mWidth is size in bytes
  if (texture->mHeight == 0) {
    int texWidth, texHeight, texChannels;
    void* pixels;
    VkDeviceSize imageSize;

    const stbi_uc* texData = reinterpret_cast<const stbi_uc*>(texture->pcData);

    if (stbi_is_hdr_from_memory(texData, texture->mWidth)) {
      // TODO(marceline-cramer) Add HDR support
      log_ftl("TextureAsset does not support HDR yet.");
    } else {
      pixels = stbi_loadf_from_memory(texData, texture->mWidth, &texWidth,
                                      &texHeight, &texChannels, STBI_rgb_alpha);

      imageSize = texWidth * texHeight * 4;

      if (!pixels) {
        log_ftl("Failed to load texture image.");
      }
    }

    image = new VulkanImage(
        vulkanInstance, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);
  } else {
    image = new VulkanImage(
        vulkanInstance, VK_FORMAT_R8G8B8A8_UNORM, texture->mWidth,
        texture->mHeight,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);
  }

  // TODO(marceline-cramer) transfer image data to VulkanImage
}

TextureAsset::~TextureAsset() {
  if (image) delete image;
}

}  // namespace mondradiko
