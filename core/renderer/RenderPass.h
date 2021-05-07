// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations;
class GpuDescriptorPool;
class GpuDescriptorSet;
class Viewport;

enum class RenderPhase : size_t {
  Pre = 0,  // Create this frame's data, cache previous frame data, etc.
  // Skin,       // Frustrum cull and skin meshes
  Depth,    // Depth prepass for faster culling in forward pass
  Overlay,  // Render UI elements, debug, etc.
  // Shadow,     // Render scene to shadow maps
  // Occlusion,  // After depth pyramid is generated, do occlusion culling
  Forward,      // Shade geometry
  Transparent,  // Transparent geometry
  // Post,       // Post-processing effects (bloom, AO, AA, etc.)
  Composite,  // Combine forward and overlay into swapchain output
  MAX         // Max enum for allocation purposes
};

class RenderPass {
 public:
  virtual ~RenderPass() {}

  /**
   * @brief Create pipelined frame data. Called on initialization.
   * @param frame_count the number of frames to pipeline.
   */
  virtual void createFrameData(uint32_t) = 0;

  /**
   * @brief Destroy pipelined frame data. Called on renderer cleanup.
   */
  virtual void destroyFrameData() = 0;

  /**
   * @brief Begins a frame. Should call addPassToPhase() to actually render.
   * @param frame_index The index of the pipelined frame to use.
   * @param viewport_count The number of viewports that will be rendered to.
   * @param descriptor_pool The descriptor pool for this frame.
   */
  virtual void beginFrame(uint32_t, uint32_t, GpuDescriptorPool*) = 0;

  /**
   * @brief Renders offscreen. Does not have access to viewport uniforms.
   * @param phase The phase of the render. Will always be below Depth.
   * @param command_buffer The command buffer to record to.
   */
  virtual void render(RenderPhase, VkCommandBuffer) = 0;

  /**
   * @brief Renders to a viewport.
   * @param command_buffer The command buffer to record to.
   * @param viewport_index The index of the viewport being rendered to.
   * @param phase The render phase. Will always be Depth or above.
   * @param viewport_descriptor The descriptor containing this viewport uniform.
   */
  virtual void renderViewport(VkCommandBuffer, uint32_t, RenderPhase,
                              const GpuDescriptorSet*) = 0;

  /**
   * @brief Ends a frame.
   */
  virtual void endFrame() = 0;

 private:
};

}  // namespace core
}  // namespace mondradiko
