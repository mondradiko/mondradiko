/**
 * @file Renderer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all Pipelines and performs frame rendering operations.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <vector>

#include "core/assets/AssetPool.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class DisplayInterface;
class GpuDescriptorPool;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;
class MeshPipeline;

struct PipelinedFrameData {
  // TODO(marceline-cramer) Use command pool per frame, per thread
  VkCommandBuffer command_buffer;
  VkSemaphore on_render_finished;
  VkFence is_in_use;

  GpuDescriptorPool* descriptor_pool;
  GpuVector* viewports;
};

struct FramePushConstant {
  uint32_t view_index;
  uint32_t material_index;
};

class Renderer {
 public:
  Renderer(DisplayInterface*, GpuInstance*);
  ~Renderer();

  void renderFrame(entt::registry&, const AssetPool*);

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  MeshPipeline* mesh_pipeline = nullptr;

 private:
  DisplayInterface* display;
  GpuInstance* gpu;

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;
};

}  // namespace mondradiko
