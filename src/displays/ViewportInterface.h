/**
 * @file ViewportInterface.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for cross-API render targets.
 * @date 2020-11-08
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

#ifndef SRC_DISPLAYS_VIEWPORTINTERFACE_H_
#define SRC_DISPLAYS_VIEWPORTINTERFACE_H_

#include "src/api_headers.h"

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

#endif  // SRC_DISPLAYS_VIEWPORTINTERFACE_H_
