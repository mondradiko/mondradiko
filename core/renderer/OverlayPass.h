// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "core/gpu/GpuPipeline.h"
#include "core/renderer/RenderPass.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class GlyphLoader;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuShader;

struct DebugDrawVertex {
  glm::vec3 position;
  glm::vec3 color;

  static GpuPipeline::VertexBindings getVertexBindings() {
    GpuPipeline::VertexBindings bindings(1);

    bindings[0] = {.binding = 0,
                   .stride = sizeof(DebugDrawVertex),
                   .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

    return bindings;
  }

  static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
    GpuPipeline::AttributeDescriptions descriptions(2);

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
  GpuShader* debug_vertex_shader = nullptr;
  GpuShader* debug_fragment_shader = nullptr;
  GpuPipeline* debug_pipeline = nullptr;

  VkPipelineLayout glyph_pipeline_layout = VK_NULL_HANDLE;
  GpuPipeline* glyph_pipeline = nullptr;

  GpuDescriptorSetLayout* glyph_set_layout = nullptr;
};

}  // namespace mondradiko
