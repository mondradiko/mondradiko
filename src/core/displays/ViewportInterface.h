/**
 * @file ViewportInterface.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for cross-API render targets.
 * @date 2020-11-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "common/api_headers.h"

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

  virtual void acquire() = 0;
  virtual void beginRenderPass(VkCommandBuffer, VkRenderPass) = 0;
  virtual void writeUniform(ViewportUniform*) = 0;
  virtual void release() = 0;
};

}  // namespace mondradiko
