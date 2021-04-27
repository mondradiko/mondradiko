// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "core/renderer/DebugDraw.h"
#include "core/renderer/RenderPass.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CVarScope;
class GlyphLoader;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuPipeline;
class GpuShader;
class GpuVector;
class Renderer;
class World;

using DebugDrawIndex = uint16_t;

class OverlayPass : public RenderPass {
 public:
  static void initCVars(CVarScope*);

  OverlayPass(const CVarScope*, const GlyphLoader*, Renderer*, World*);
  ~OverlayPass();

  DebugDrawList* getDebugDraw() { return &_debug_draw; }

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

  DebugDrawList _debug_draw;

  struct FrameData {
    GpuVector* debug_vertices = nullptr;
    GpuVector* debug_indices = nullptr;

    uint16_t index_count;
  };

  types::vector<FrameData> frame_data;
  uint32_t current_frame;
};

}  // namespace core
}  // namespace mondradiko
