/**
 * @file OpenXrViewport.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an OpenXR swapchain and view.
 * @date 2020-11-09
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/openxr_headers.h"
#include "core/displays/ViewportInterface.h"

namespace mondradiko {

// Forward declaration
class GpuImage;
class GpuInstance;
class OpenXrDisplay;
class Renderer;

class OpenXrViewport : public ViewportInterface {
 public:
  OpenXrViewport(GpuInstance*, OpenXrDisplay*, Renderer*,
                 XrViewConfigurationView*);
  ~OpenXrViewport();

  VkSemaphore acquire() final;
  void beginRenderPass(VkCommandBuffer, VkRenderPass) final;
  void writeUniform(ViewportUniform*) final;
  void release(VkSemaphore) final;

  bool requiresSignal() final { return false; }

  void updateView(const XrView&);
  void writeCompositionLayers(XrCompositionLayerProjectionView*);

 private:
  GpuInstance* gpu;
  OpenXrDisplay* display;
  Renderer* renderer;

  XrSwapchain swapchain = XR_NULL_HANDLE;

  XrView view;

  std::vector<ViewportImage> images;
  GpuImage* depth_image = nullptr;
  uint32_t current_image_index;
  uint32_t image_width;
  uint32_t image_height;
};

}  // namespace mondradiko
