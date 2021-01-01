/**
 * @file Renderer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all Pipelines and performs frame rendering operations.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <vector>

#include "core/assets/AssetPool.h"
#include "core/renderer/RenderPass.h"

namespace mondradiko {

// Forward declarations
class DisplayInterface;
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
  Renderer(DisplayInterface*, GpuInstance*);
  ~Renderer();

  void renderFrame(EntityRegistry&, AssetPool*);

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  MeshPass* mesh_pass = nullptr;
  OverlayPass* overlay_pass = nullptr;

 private:
  DisplayInterface* display;
  GpuInstance* gpu;

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;
};

}  // namespace mondradiko
