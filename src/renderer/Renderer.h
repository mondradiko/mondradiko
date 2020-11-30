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

#ifndef SRC_RENDERER_RENDERER_H_
#define SRC_RENDERER_RENDERER_H_

#include <vector>

#include "displays/DisplayInterface.h"
#include "gpu/GpuDescriptorPool.h"
#include "gpu/GpuInstance.h"
#include "renderer/MeshPipeline.h"
#include "src/api_headers.h"

namespace mondradiko {

struct PipelinedFrameData {
  // TODO(marceline-cramer) Use command pool per frame, per thread
  VkCommandBuffer command_buffer;
  VkFence is_in_use;
  bool submitted;

  GpuDescriptorPool* descriptor_pool;
  GpuBuffer* viewports;
};

struct FramePushConstant {
  uint32_t view_index;
  uint32_t material_index;
};

class Renderer {
 public:
  Renderer(DisplayInterface*, GpuInstance*);
  ~Renderer();

  void renderFrame();

  VkRenderPass composite_pass = VK_NULL_HANDLE;

  GpuDescriptorSetLayout* viewport_layout = nullptr;

  MeshPipeline* mesh_pipeline = nullptr;

 private:
  DisplayInterface* display;
  GpuInstance* gpu;

  uint32_t current_frame = 0;
  std::vector<PipelinedFrameData> frames_in_flight;

  void findSwapchainFormat();
  void createRenderPasses();
  void createDescriptorSetLayout();
  void createFrameData();
  void createPipelines();
};

}  // namespace mondradiko

#endif  // SRC_RENDERER_RENDERER_H_
