/**
 * @file TextureAsset.h
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

#ifndef SRC_ASSETS_TEXTUREASSET_H_
#define SRC_ASSETS_TEXTUREASSET_H_

#include <string>

#include "assets/Asset.h"
#include "graphics/VulkanImage.h"
#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class TextureAsset : public Asset {
 public:
  TextureAsset(VulkanInstance*, aiTexture*, VkSampler);
  ~TextureAsset();

  /**
   * @brief Stores the index into the CURRENT FRAME's descriptor array.
   *
   */
  uint32_t index = 0;

  VulkanImage* image = nullptr;
  VkSampler sampler;

 private:
  VulkanInstance* vulkan_instance;
};

}  // namespace mondradiko

#endif  // SRC_ASSETS_TEXTUREASSET_H_
