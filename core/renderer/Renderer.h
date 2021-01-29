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

struct FramePushConstant {
  uint32_t view_index;
  uint32_t material_index;
};

class Renderer {
 public:
  static void initCVars(CVarScope*);

  Renderer(const CVarScope*, DisplayInterface*, const GlyphLoader*,
           GpuInstance*);
  ~Renderer();

  void renderFrame(EntityRegistry&, AssetPool*);

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  MeshPass* mesh_pass = nullptr;
  OverlayPass* overlay_pass = nullptr;

 private:
  const CVarScope* cvars;
  DisplayInterface* display;
  const GlyphLoader* glyphs;
  GpuInstance* gpu;

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;
};

}  // namespace mondradiko
