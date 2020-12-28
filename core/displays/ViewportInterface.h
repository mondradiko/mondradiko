/**
 * @file ViewportInterface.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for cross-API render targets.
 * @date 2020-11-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/common/glm_headers.h"
#include "core/common/vulkan_headers.h"

namespace mondradiko {

struct ViewportUniform {
  glm::mat4 view;
  glm::mat4 projection;
};

struct ViewportImage {
  VkImage image;
  VkImageView image_view;
  VkFramebuffer framebuffer;
};

class ViewportInterface {
 public:
  virtual ~ViewportInterface() {}

  virtual VkSemaphore acquire() = 0;
  virtual void beginRenderPass(VkCommandBuffer, VkRenderPass) = 0;
  virtual void writeUniform(ViewportUniform*) = 0;
  virtual void release(VkSemaphore) = 0;
};

}  // namespace mondradiko
