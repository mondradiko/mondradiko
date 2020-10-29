/**
 * @file Viewport.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to cameras, from OpenXR eyes or otherwise.
 * @date 2020-10-24
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

#ifndef SRC_GRAPHICS_VIEWPORT_H_
#define SRC_GRAPHICS_VIEWPORT_H_

#include <vector>

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"
#include "xr/PlayerSession.h"

namespace mondradiko {

struct ViewportImage {
  VkImage image;
  VkImageView imageView;
  VkFramebuffer framebuffer;
};

class Viewport {
 public:
  Viewport(VkFormat, VkRenderPass, XrViewConfigurationView*, PlayerSession*,
           VulkanInstance*);
  ~Viewport();

  void acquireSwapchainImage();
  void beginRenderPass(VkCommandBuffer, VkRenderPass);
  void setCommandViewport(VkCommandBuffer);
  void releaseSwapchainImage();

  XrSwapchain swapchain = XR_NULL_HANDLE;
  std::vector<ViewportImage> images;

  uint32_t width;
  uint32_t height;

 private:
  PlayerSession* session;
  VulkanInstance* vulkanInstance;

  uint32_t currentIndex;
};

}  // namespace mondradiko

#endif  // SRC_GRAPHICS_VIEWPORT_H_
