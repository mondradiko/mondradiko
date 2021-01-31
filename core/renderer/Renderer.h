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
class GlyphLoader;
class GpuDescriptorPool;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;
class MeshPass;
class OverlayPass;
class UserInterface;
class World;

class Renderer {
 public:
  static void initCVars(CVarScope*);

  Renderer(const CVarScope*, DisplayInterface*, const GlyphLoader*,
           GpuInstance*, World*);
  ~Renderer();

  void renderFrame();

  VkRenderPass getCompositePass() { return composite_pass; }

 private:
  const CVarScope* cvars;
  DisplayInterface* display;
  const GlyphLoader* glyphs;
  GpuInstance* gpu;
  World* world;

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  MeshPass* mesh_pass = nullptr;
  OverlayPass* overlay_pass = nullptr;

  struct PipelinedFrameData {
    // TODO(marceline-cramer) Use command pool per frame, per thread
    VkCommandBuffer command_buffer;
    VkSemaphore on_render_finished;
    VkFence is_in_use;

    GpuDescriptorPool* descriptor_pool;
    GpuVector* viewports;
  };

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;
};

}  // namespace mondradiko
