/**
 * @file PlayerSession.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Handles the lifetime of an OpenXR session, receives events, manages
 * viewports, synchronizes frames, and creates inputs.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_XR_PLAYERSESSION_H_
#define SRC_XR_PLAYERSESSION_H_

#include <vector>

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"
#include "xr/XrDisplay.h"

// Forward declarations because of codependency
class Renderer;
class Viewport;

class PlayerSession {
 public:
  PlayerSession(XrDisplay*, VulkanInstance*);
  ~PlayerSession();

  void pollEvents(bool*, bool*);
  void beginFrame(double*, bool*);
  void endFrame(Renderer*, bool);

  void enumerateSwapchainFormats(std::vector<VkFormat>*);
  bool createViewports(std::vector<Viewport*>*, VkFormat, VkRenderPass);

  XrSession session = XR_NULL_HANDLE;
  XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
  XrFrameState currentFrameState;

  XrSpace stageSpace = XR_NULL_HANDLE;

  std::vector<XrView> views;

 private:
  XrDisplay* display;
  VulkanInstance* vulkanInstance;
};

#endif  // SRC_XR_PLAYERSESSION_H_
