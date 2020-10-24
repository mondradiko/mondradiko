/**
 * @file MeshAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains the geometry for a mesh.
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

#ifndef SRC_ASSETS_MESHASSET_H_
#define SRC_ASSETS_MESHASSET_H_

#include <array>
#include <string>

#include "src/api_headers.h"
#include "assets/Asset.h"
#include "graphics/VulkanBuffer.h"
#include "graphics/VulkanInstance.h"

using MeshVertexAttributeDescriptions =
    std::array<VkVertexInputAttributeDescription, 3>;

struct MeshVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription description{
        .binding = 0,
        .stride = sizeof(MeshVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

    return description;
  }

  static MeshVertexAttributeDescriptions getAttributeDescriptions() {
    MeshVertexAttributeDescriptions descriptions;

    descriptions[0] = {.location = 0,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32B32_SFLOAT,
                       .offset = offsetof(MeshVertex, position)};

    descriptions[1] = {.location = 1,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32B32_SFLOAT,
                       .offset = offsetof(MeshVertex, normal)};

    descriptions[2] = {.location = 2,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32_SFLOAT,
                       .offset = offsetof(MeshVertex, texCoord)};

    return descriptions;
  }
};

using MeshIndex = uint32_t;

class MeshAsset : public Asset {
 public:
  MeshAsset(std::string, VulkanInstance*, aiMesh*);
  ~MeshAsset();

  VulkanBuffer* vertexBuffer = nullptr;
  VulkanBuffer* indexBuffer = nullptr;
  size_t indexCount = 0;

  std::string meshName = nullptr;

 private:
  VulkanInstance* vulkanInstance;
};

#endif  // SRC_ASSETS_MESHASSET_H_
