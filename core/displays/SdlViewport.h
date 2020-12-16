/**
 * @file SdlViewport.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an SDL window swapchain.
 * @date 2020-11-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/displays/ViewportInterface.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"

namespace mondradiko {

// Forward declaration because of codependence
class SdlDisplay;

class SdlViewport : public ViewportInterface {
 public:
  SdlViewport(GpuInstance*, SdlDisplay*, Renderer*);
  ~SdlViewport();

  VkSemaphore acquire() final;
  void beginRenderPass(VkCommandBuffer, VkRenderPass) final;
  void writeUniform(ViewportUniform*) final;
  void release(VkSemaphore) final;

 private:
  GpuInstance* gpu;
  SdlDisplay* display;
  Renderer* renderer;

  uint32_t acquire_image_index;
  std::vector<VkSemaphore> on_image_acquire;

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  std::vector<ViewportImage> images;
  uint32_t current_image_index;
  uint32_t image_width;
  uint32_t image_height;
};

}  // namespace mondradiko
