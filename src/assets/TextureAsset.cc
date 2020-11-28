/**
 * @file TextureAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains a texture, intended for mapping to meshes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/TextureAsset.h"

#include "log/log.h"

namespace mondradiko {

TextureAsset::TextureAsset(GpuInstance* gpu, aiTexture* texture,
                           VkSampler sampler)
    : sampler(sampler), gpu(gpu) {
  // By default, load directly from the embedded texture
  void* texData = texture->pcData;
  VkFormat texFormat = VK_FORMAT_R8G8B8A8_SRGB;
  int texWidth = texture->mWidth;
  int texHeight = texture->mHeight;

  // If the height of the texture is 0,
  // that means that it's stored in a compressed format
  // like PNG, JPG, etc., and mWidth is size in bytes
  if (texture->mHeight == 0) {
    int texChannels;
    const stbi_uc* embeddedData =
        reinterpret_cast<const stbi_uc*>(texture->pcData);

    if (stbi_is_hdr_from_memory(embeddedData, texture->mWidth)) {
      // TODO(marceline-cramer) Add HDR support
      log_ftl("TextureAsset does not support HDR yet.");
    } else {
      texData = stbi_load_from_memory(embeddedData, texture->mWidth, &texWidth,
                                      &texHeight, &texChannels, STBI_rgb_alpha);

      if (!texData) {
        log_ftl("Failed to load texture image.");
      }
    }
  }

  image = new GpuImage(
      gpu, texFormat, texWidth, texHeight,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VMA_MEMORY_USAGE_GPU_ONLY);

  image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  image->writeData(texData);
  image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  image->createView();

  // If we loaded from stbi, free the memory
  if (texture->mHeight == 0) {
    stbi_image_free(texData);
  }
}

TextureAsset::~TextureAsset() {
  if (image) delete image;
}

}  // namespace mondradiko
