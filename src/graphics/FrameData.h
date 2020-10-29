/**
 * @file FrameData.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Temp class to encapsulate pipelined frame data.
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

#ifndef SRC_GRAPHICS_FRAMEDATA_H_
#define SRC_GRAPHICS_FRAMEDATA_H_

#include <vector>

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

struct FrameInFlight {
  VkCommandBuffer commandBuffer;
  VkFence isInUse;
  bool submitted;
};

class FrameData {
 public:
  FrameData(VulkanInstance*, uint32_t);
  ~FrameData();

  VkCommandBuffer beginPrimaryCommand();
  void endPrimaryCommand();
  void submitPrimaryCommand();

 private:
  VulkanInstance* vulkanInstance;

  uint32_t currentFrame = 0;
  std::vector<FrameInFlight> framesInFlight;
};

}  // namespace mondradiko

#endif  // SRC_GRAPHICS_FRAMEDATA_H_
