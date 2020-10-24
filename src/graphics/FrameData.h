/**
 * @file FrameData.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Temp class to encapsulate pipelined frame data.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_GRAPHICS_FRAMEDATA_H_
#define SRC_GRAPHICS_FRAMEDATA_H_

#include <vector>

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

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

#endif  // SRC_GRAPHICS_FRAMEDATA_H_
