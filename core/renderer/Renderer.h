// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "core/renderer/RenderPass.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CVarScope;
class Display;
class GpuBuffer;
class GpuDescriptorPool;
class GpuDescriptorSetLayout;
class GpuImage;
class GpuInstance;
class GpuVector;

class Renderer {
 public:
  static void initCVars(CVarScope*);

  Renderer(const CVarScope*, Display*, GpuInstance*);
  ~Renderer();

  void addRenderPass(RenderPass*);
  void destroyFrameData();

  void renderFrame();
  void addPassToPhase(RenderPhase, RenderPass*);

  // TODO(marceline-cramer) TransferWorker
  void transferDataToBuffer(GpuBuffer*, size_t, const void*, size_t);
  void transferDataToImage(GpuImage*, const void*);

  GpuInstance* getGpu() { return gpu; }
  GpuDescriptorSetLayout* getViewportLayout() { return viewport_layout; }

  // TODO(marceline-cramer) Wrapper class for subpass manipulation
  VkRenderPass getViewportRenderPass() const { return render_pass; }
  uint32_t getDepthSubpass() { return 0; }
  uint32_t getForwardSubpass() { return 1; }
  uint32_t getTransparentSubpass() { return 2; }

  GpuImage* getErrorImage() { return error_image; }

 private:
  const CVarScope* cvars;
  Display* display;
  GpuInstance* gpu;

  VkRenderPass render_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  types::vector<RenderPass*> render_passes;

  struct PipelinedFrameData {
    // TODO(marceline-cramer) Use command pool per frame, per thread
    VkCommandBuffer command_buffer;
    VkSemaphore on_render_finished;
    VkFence is_in_use;

    GpuDescriptorPool* descriptor_pool;
    GpuVector* viewports;

    using PassList = types::vector<RenderPass*>;
    static constexpr auto PhaseNum = static_cast<size_t>(RenderPhase::MAX);
    using PhaseList = std::array<PassList, PhaseNum>;
    PhaseList phases;
  };

  uint32_t current_frame = 0;
  types::vector<PipelinedFrameData> frames_in_flight;

  GpuImage* error_image = nullptr;
};

}  // namespace core
}  // namespace mondradiko
