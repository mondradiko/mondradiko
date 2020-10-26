/**
 * @file TextureAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains a texture, intended for mapping to meshes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "assets/TextureAsset.h"

#include "graphics/VulkanImage.h"
#include "log/log.h"

TextureAsset::TextureAsset(VulkanInstance* vulkanInstance, aiTexture* texture)
    : vulkanInstance(vulkanInstance) {
  image = new VulkanImage(
      vulkanInstance, VK_FORMAT_R8G8B8A8_UNORM, texture->mWidth,
      texture->mHeight,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VMA_MEMORY_USAGE_GPU_ONLY);
}

TextureAsset::~TextureAsset() {
  if (image) delete image;
}
