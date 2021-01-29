// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "core/renderer/RenderPass.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class GlyphLoader;
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

using GlyphInstanceAttributeDescriptions =
    std::array<VkVertexInputAttributeDescription, 2>;

struct GlyphInstance {
  glm::vec2 position;
  uint32_t glyph_index;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription description{
        .binding = 0,
        .stride = sizeof(GlyphInstance),
        .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE};

    return description;
  }

  static GlyphInstanceAttributeDescriptions getAttributeDescriptions() {
    GlyphInstanceAttributeDescriptions descriptions;

    descriptions[0] = {.location = 0,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32_SFLOAT,
                       .offset = offsetof(GlyphInstance, position)};

    descriptions[1] = {.location = 1,
                       .binding = 0,
                       .format = VK_FORMAT_R32_UINT,
                       .offset = offsetof(GlyphInstance, glyph_index)};

    return descriptions;
  }
};

class OverlayPass {
 public:
  static void initCVars(CVarScope*);

  OverlayPass(const CVarScope*, const GlyphLoader*, GpuInstance*,
              GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~OverlayPass();

  void createFrameData(OverlayPassFrameData&);
  void destroyFrameData(OverlayPassFrameData&);
  void allocateDescriptors(EntityRegistry&, OverlayPassFrameData&, AssetPool*,
                           GpuDescriptorPool*);
  void render(EntityRegistry&, OverlayPassFrameData&, AssetPool*,
              VkCommandBuffer, GpuDescriptorSet*, uint32_t);

 private:
  const CVarScope* cvars;
  const GlyphLoader* glyphs;
  GpuInstance* gpu;

  VkPipelineLayout debug_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline debug_pipeline = VK_NULL_HANDLE;

  VkPipelineLayout glyph_pipeline_layout = VK_NULL_HANDLE;
  VkPipeline glyph_pipeline = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* glyph_set_layout = nullptr;
};

}  // namespace mondradiko
