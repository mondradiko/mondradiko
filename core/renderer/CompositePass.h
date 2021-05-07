// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/renderer/RenderPass.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuDescriptorSet;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuPipeline;
class GpuShader;
class Renderer;

class CompositePass : public RenderPass {
 public:
  CompositePass(Renderer*);
  ~CompositePass();

  // RenderPass implementation
  void createFrameData(uint32_t) final;
  void destroyFrameData() final {}

  void beginFrame(uint32_t, uint32_t, GpuDescriptorPool*) final;
  void render(RenderPhase, VkCommandBuffer) final {}
  void renderViewport(VkCommandBuffer, uint32_t, RenderPhase,
                      const GpuDescriptorSet*) final;
  void endFrame() final {}

 private:
  GpuInstance* gpu;
  Renderer* renderer;

  GpuDescriptorSetLayout* _set_layout = nullptr;
  VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
  GpuShader* _vertex_shader = nullptr;
  GpuShader* _fragment_shader = nullptr;
  GpuPipeline* _pipeline = nullptr;

  struct FrameData {
    types::vector<GpuDescriptorSet*> input_attachments;  // One per viewport
  };

  types::vector<FrameData> _frame_data;
  uint32_t _current_frame;
};

}  // namespace core
}  // namespace mondradiko
