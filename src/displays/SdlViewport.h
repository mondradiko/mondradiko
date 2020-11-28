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

#ifndef SRC_DISPLAYS_SDLVIEWPORT_H_
#define SRC_DISPLAYS_SDLVIEWPORT_H_

#include <vector>

#include "displays/ViewportInterface.h"
#include "gpu/GpuInstance.h"
#include "renderer/Renderer.h"

namespace mondradiko {

// Forward declaration because of codependence
class SdlDisplay;

class SdlViewport : public ViewportInterface {
 public:
  SdlViewport(GpuInstance*, SdlDisplay*, Renderer*);
  ~SdlViewport();

  void acquire() override;
  void beginRenderPass(VkCommandBuffer, VkRenderPass) override;
  void writeUniform(ViewportUniform*) override;
  void release() override;

 private:
  GpuInstance* gpu;
  SdlDisplay* display;
  Renderer* renderer;

  VkFence on_image_available = VK_NULL_HANDLE;

  std::vector<ViewportImage> images;
  uint32_t current_image_index;
  uint32_t image_width;
  uint32_t image_height;
};

}  // namespace mondradiko

#endif  // SRC_DISPLAYS_SDLVIEWPORT_H_
