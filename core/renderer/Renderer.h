// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <vector>

#include "core/assets/AssetPool.h"
#include "core/renderer/RenderPass.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class DisplayInterface;
class GpuDescriptorPool;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;

class Renderer {
 public:
  static void initCVars(CVarScope*);

  Renderer(const CVarScope*, DisplayInterface*, GpuInstance*);
  ~Renderer();

  void addRenderPass(RenderPass*);
  void destroyFrameData();

  void renderFrame();
  void addPassToPhase(RenderPhase, RenderPass*);

  GpuInstance* getGpu() { return gpu; }
  GpuDescriptorSetLayout* getViewportLayout() { return viewport_layout; }
  VkRenderPass getCompositePass() const { return composite_pass; }

 private:
  const CVarScope* cvars;
  DisplayInterface* display;
  GpuInstance* gpu;

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  std::vector<RenderPass*> render_passes;

  struct PipelinedFrameData {
    // TODO(marceline-cramer) Use command pool per frame, per thread
    VkCommandBuffer command_buffer;
    VkSemaphore on_render_finished;
    VkFence is_in_use;

    GpuDescriptorPool* descriptor_pool;
    GpuVector* viewports;

    using PassList = std::vector<RenderPass*>;
    static constexpr size_t PhaseNum = static_cast<size_t>(RenderPhase::MAX);
    using PhaseList = std::array<PassList, PhaseNum>;
    PhaseList phases;
  };

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;
};

}  // namespace mondradiko
