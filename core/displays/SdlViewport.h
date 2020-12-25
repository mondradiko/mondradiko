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

#include "core/common/sdl2_headers.h"
#include "core/displays/ViewportInterface.h"

namespace mondradiko {

// Forward declarations
class GpuImage;
class GpuInstance;
class Renderer;
class SdlDisplay;

class SdlViewport : public ViewportInterface {
 public:
  SdlViewport(GpuInstance*, SdlDisplay*, Renderer*);
  ~SdlViewport();

  VkSemaphore acquire() final;
  void beginRenderPass(VkCommandBuffer, VkRenderPass) final;
  void writeUniform(ViewportUniform*) final;
  void release(VkSemaphore) final;

  void moveCamera(float, float, float, float, float);

 private:
  GpuInstance* gpu;
  SdlDisplay* display;
  Renderer* renderer;

  uint32_t acquire_image_index;
  std::vector<VkSemaphore> on_image_acquire;

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  GpuImage* depth_image = nullptr;
  std::vector<ViewportImage> images;
  uint32_t current_image_index;
  uint32_t image_width;
  uint32_t image_height;

  glm::vec3 camera_position;
  float camera_pan;
  float camera_tilt;
};

}  // namespace mondradiko
