// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>
#include <vector>

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
class GpuVector;
class Renderer;
class World;

struct DebugDrawVertex {
  glm::vec3 position;
  glm::vec3 color;

  static GpuPipeline::VertexBindings getVertexBindings() {
    // VkVertexInputBindingDescription{binding, stride, inputRate}
    return {
        {0, sizeof(DebugDrawVertex), VK_VERTEX_INPUT_RATE_VERTEX},
    };
  }

  static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
    // VkVertexInputAttributeDescription{location, binding, format, offset}
    return {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugDrawVertex, position)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugDrawVertex, color)},
    };
  }
};

using DebugDrawIndex = uint16_t;

class OverlayPass : public RenderPass {
 public:
  static void initCVars(CVarScope*);

  OverlayPass(const CVarScope*, const GlyphLoader*, Renderer*, World*);
  ~OverlayPass();

  // RenderPass implementation
  void createFrameData(uint32_t) final;
  void destroyFrameData() final;
  void beginFrame(uint32_t, GpuDescriptorPool*) final;
  void render(RenderPhase, VkCommandBuffer) final {}
  void renderViewport(RenderPhase, VkCommandBuffer,
                      const GpuDescriptorSet*) final;
  void endFrame() final {}

 private:
  const CVarScope* cvars;
  const GlyphLoader* glyphs;
  GpuInstance* gpu;
  Renderer* renderer;
  World* world;

  VkPipelineLayout debug_pipeline_layout = VK_NULL_HANDLE;
  GpuShader* debug_vertex_shader = nullptr;
  GpuShader* debug_fragment_shader = nullptr;
  GpuPipeline* debug_pipeline = nullptr;

  VkPipelineLayout glyph_pipeline_layout = VK_NULL_HANDLE;
  GpuPipeline* glyph_pipeline = nullptr;

  GpuDescriptorSetLayout* glyph_set_layout = nullptr;

  struct FrameData {
    GpuVector* debug_vertices = nullptr;
    GpuVector* debug_indices = nullptr;

    uint16_t index_count;

    GpuDescriptorSet* glyph_descriptor = nullptr;
    GpuVector* glyph_instances = nullptr;
    uint32_t glyph_count;
  };

  std::vector<FrameData> frame_data;
  uint32_t current_frame;
};

}  // namespace mondradiko
