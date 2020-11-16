/**
 * @file OpenXrViewport.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an OpenXR swapchain and view.
 * @date 2020-11-09
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

#ifndef SRC_DISPLAYS_OPENXRVIEWPORT_H_
#define SRC_DISPLAYS_OPENXRVIEWPORT_H_

#include <vector>

#include "displays/ViewportInterface.h"
#include "gpu/GpuInstance.h"

namespace mondradiko {

// Forward declaration because of codependence
class OpenXrDisplay;

class OpenXrViewport : public ViewportInterface {
 public:
  OpenXrViewport(GpuInstance*, OpenXrDisplay*, Renderer*,
                 XrViewConfigurationView*);
  ~OpenXrViewport();

  void acquire();
  void beginRenderPass(VkCommandBuffer, VkRenderPass) override;
  void writeUniform(ViewportUniform*) override;
  void release() override;

  void updateView(const XrView&);
  void writeCompositionLayers(XrCompositionLayerProjectionView*);

 private:
  GpuInstance* gpu;
  OpenXrDisplay* display;
  Renderer* renderer;

  XrSwapchain swapchain = XR_NULL_HANDLE;

  XrView view;

  std::vector<ViewportImage> images;
  uint32_t current_image_index;
  uint32_t image_width;
  uint32_t image_height;
};

}  // namespace mondradiko

#endif  // SRC_DISPLAYS_OPENXRVIEWPORT_H_
