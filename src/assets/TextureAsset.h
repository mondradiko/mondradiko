/**
 * @file TextureAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains a texture, intended for mapping to meshes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_ASSETS_TEXTUREASSET_H_
#define SRC_ASSETS_TEXTUREASSET_H_

#include <string>

#include "assets/Asset.h"
#include "src/api_headers.h"

class VulkanImage;
class VulkanInstance;

class TextureAsset : public Asset {
 public:
  TextureAsset(VulkanInstance*, aiTexture*);
  ~TextureAsset();

  VulkanImage* image = nullptr;

 private:
  VulkanInstance* vulkanInstance;
};

#endif  // SRC_ASSETS_TEXTUREASSET_H_