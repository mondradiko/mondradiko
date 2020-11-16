/**
 * @file MeshAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains the geometry for a mesh.
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

#ifndef SRC_ASSETS_MESHASSET_H_
#define SRC_ASSETS_MESHASSET_H_

#include <array>
#include <string>

#include "assets/Asset.h"
#include "gpu/GpuBuffer.h"
#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

using MeshVertexAttributeDescriptions =
    std::array<VkVertexInputAttributeDescription, 3>;

struct MeshVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;

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
                       .offset = offsetof(MeshVertex, tex_coord)};

    return descriptions;
  }
};

using MeshIndex = uint32_t;

class MeshAsset : public Asset {
 public:
  MeshAsset(std::string, GpuInstance*, aiMesh*);
  ~MeshAsset();

  GpuBuffer* vertex_buffer = nullptr;
  GpuBuffer* index_buffer = nullptr;
  size_t index_count = 0;

  std::string mesh_name = nullptr;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko

#endif  // SRC_ASSETS_MESHASSET_H_
