/**
 * @file MeshAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains the geometry for a mesh.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <array>
#include <string>

#include "assets/Asset.h"
#include "gpu/GpuBuffer.h"
#include "gpu/GpuInstance.h"
#include "common/api_headers.h"

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
