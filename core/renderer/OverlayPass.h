// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "core/renderer/RenderPass.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {

// Forward declarations
class GpuDescriptorSetLayout;
class GpuInstance;

using DebugDrawVertexAttributeDescriptions =
    std::array<VkVertexInputAttributeDescription, 2>;

struct DebugDrawVertex {
  glm::vec3 position;
  glm::vec3 color;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription description{
        .binding = 0,
        .stride = sizeof(DebugDrawVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

    return description;
  }

  static DebugDrawVertexAttributeDescriptions getAttributeDescriptions() {
    DebugDrawVertexAttributeDescriptions descriptions;

    descriptions[0] = {.location = 0,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32B32_SFLOAT,
                       .offset = offsetof(DebugDrawVertex, position)};

    descriptions[1] = {.location = 1,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32B32_SFLOAT,
                       .offset = offsetof(DebugDrawVertex, color)};

    return descriptions;
  }
};

using DebugDrawIndex = uint16_t;

class OverlayPass {
 public:
  OverlayPass(GpuInstance*, GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~OverlayPass();

  void createFrameData(OverlayPassFrameData&);
  void destroyFrameData(OverlayPassFrameData&);
  void allocateDescriptors(EntityRegistry&, OverlayPassFrameData&, AssetPool*,
                           GpuDescriptorPool*);
  void render(EntityRegistry&, OverlayPassFrameData&, AssetPool*,
              VkCommandBuffer, GpuDescriptorSet*, uint32_t);

  VkPipelineLayout debug_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline debug_pipeline = VK_NULL_HANDLE;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko
