// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "core/renderer/RenderPass.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CompositePass;
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

  Viewport* getCurrentViewport(uint32_t viewport_index) {
    return frames_in_flight[current_frame].viewports[viewport_index];
  }

  // TODO(marceline-cramer) Wrapper class for subpass manipulation
  VkRenderPass getMainRenderPass() const { return _main_rp; }
  uint32_t getDepthSubpass() { return 0; }
  uint32_t getOverlaySubpass() { return 1; }
  uint32_t getForwardSubpass() { return 2; }
  uint32_t getTransparentSubpass() { return 3; }

  VkRenderPass getCompositeRenderPass() const { return _composite_rp; }
  uint32_t getCompositeSubpass() { return 0; }

  GpuImage* getErrorImage() { return error_image; }

 private:
  const CVarScope* cvars;
  Display* display;
  GpuInstance* gpu;

  VkRenderPass _main_rp = VK_NULL_HANDLE;
  VkRenderPass _composite_rp = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  CompositePass* composite_pass = nullptr;

  types::vector<RenderPass*> render_passes;

  struct PipelinedFrameData {
    // TODO(marceline-cramer) Use command pool per frame, per thread
    VkCommandBuffer main_commands;
    VkSemaphore on_render_finished;

    VkCommandBuffer composite_commands;
    VkSemaphore on_composite_finished;

    VkFence is_in_use;

    types::vector<Viewport*> viewports;

    GpuDescriptorPool* descriptor_pool;
    GpuVector* viewport_data;

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
