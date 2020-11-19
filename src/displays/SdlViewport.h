/**
 * @file SdlViewport.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an SDL window swapchain.
 * @date 2020-11-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
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
