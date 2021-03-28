// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>
#include <cstddef>

#include "core/assets/AssetPool.h"
#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"
#include "lib/include/vulkan_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuInstance;
class GpuBuffer;
class MeshPass;

struct MeshVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 color;
  glm::vec2 tex_coord;

  static GpuPipeline::VertexBindings getVertexBindings() {
    // VkVertexInputBindingDescription{binding, stride, inputRate}
    return {
        {0, sizeof(MeshVertex), VK_VERTEX_INPUT_RATE_VERTEX},
    };
  }

  static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
    // VkVertexInputAttributeDescription{location, binding, format, offset}
    return {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, position)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, normal)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, tangent)},
        {3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, color)},
        {4, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(MeshVertex, tex_coord)},
    };
  }
};

using MeshIndex = uint32_t;

class MeshAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::MeshAsset);

  // Asset lifetime implementation
  MeshAsset() : mesh_pass(nullptr) {}
  explicit MeshAsset(MeshPass* mesh_pass) : mesh_pass(mesh_pass) {}
  void load(const assets::SerializedAsset*) final;

  size_t getVertexOffset() const { return vertex_offset; }
  size_t getIndexOffset() const { return index_offset; }
  size_t getIndexNum() const { return index_num; }

 private:
  MeshPass* mesh_pass;

  size_t vertex_offset = 0;
  size_t index_offset = 0;
  size_t index_num = 0;
};

}  // namespace core
}  // namespace mondradiko
