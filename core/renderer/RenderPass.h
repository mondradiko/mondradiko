// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"

namespace mondradiko {

// Forward declarations;
class GpuDescriptorPool;
class GpuDescriptorSet;

enum class RenderPhase : size_t {
  Pre = 0,  // Create this frame's data, cache previous frame data, etc.
  // Skin,       // Frustrum cull and skin meshes
  // Depth,  // Depth prepass for faster culling in forward pass
  // Shadow,     // Render scene to shadow maps
  // Occlusion,  // After depth pyramid is generated, do occlusion culling
  Forward,  // Shade geometry
  // Overlay,  // Render UI elements, debug, etc.
  // Post,       // Post-processing effects (bloom, AO, AA, etc.)
  // Composite,  // Combine forward and overlay into swapchain output
  MAX  // Max enum for allocation purposes
};

class RenderPass {
 public:
  virtual ~RenderPass() {}

  virtual void createFrameData(uint32_t) = 0;
  virtual void destroyFrameData() = 0;

  virtual void beginFrame(uint32_t, GpuDescriptorPool*) = 0;
  virtual void render(RenderPhase, VkCommandBuffer) = 0;
  virtual void renderViewport(RenderPhase, VkCommandBuffer,
                              const GpuDescriptorSet*) = 0;
  virtual void endFrame() = 0;

 private:
};

}  // namespace mondradiko
