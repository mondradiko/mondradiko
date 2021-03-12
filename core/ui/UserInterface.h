// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/renderer/RenderPass.h"
#include "types/containers/vector.h"

namespace mondradiko {

// Forward declarations
class GlyphLoader;
class GpuImage;
class GpuInstance;
class GpuPipeline;
class GpuShader;
class Renderer;
class ScriptEnvironment;
class UiPanel;

class UserInterface : public RenderPass {
 public:
  UserInterface(GlyphLoader*, Renderer*);
  ~UserInterface();

  // RenderPass implementation
  void createFrameData(uint32_t) final;
  void destroyFrameData() final;
  void beginFrame(uint32_t, GpuDescriptorPool*) final;
  void render(RenderPhase, VkCommandBuffer) final {}
  void renderViewport(RenderPhase, VkCommandBuffer,
                      const GpuDescriptorSet*) final;
  void endFrame() final {}

 private:
  GlyphLoader* glyphs;
  GpuInstance* gpu;
  Renderer* renderer;

  ScriptEnvironment* scripts = nullptr;

  UiPanel* main_panel = nullptr;

  GpuShader* panel_vertex_shader = nullptr;
  GpuShader* panel_fragment_shader = nullptr;
  VkPipelineLayout panel_pipeline_layout = VK_NULL_HANDLE;
  GpuPipeline* panel_pipeline = nullptr;

  struct FrameData {
    GpuImage* panel_atlas = nullptr;
  };

  types::vector<FrameData> frame_data;
  uint32_t current_frame;
};

}  // namespace mondradiko
