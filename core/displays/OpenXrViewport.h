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

#include "core/displays/Viewport.h"
#include "lib/include/openxr_headers.h"

namespace mondradiko {

// Forward declaration
class GpuImage;
class GpuInstance;
class OpenXrDisplay;
class Renderer;

class OpenXrViewport : public Viewport {
 public:
  OpenXrViewport(GpuInstance*, OpenXrDisplay*, Renderer*,
                 XrViewConfigurationView*);
  ~OpenXrViewport();

  // Viewport implementation
  void writeUniform(ViewportUniform*) final;
  bool isSignalRequired() final { return false; }

  void updateView(const XrView&);
  void writeCompositionLayers(XrCompositionLayerProjectionView*);

 private:
  GpuInstance* gpu;
  OpenXrDisplay* display;
  Renderer* renderer;

  // Viewport implementation
  VkSemaphore _acquireImage(uint32_t*) final;
  void _releaseImage(uint32_t, VkSemaphore) final;

  XrSwapchain swapchain = XR_NULL_HANDLE;
  XrView view;
};

}  // namespace mondradiko
