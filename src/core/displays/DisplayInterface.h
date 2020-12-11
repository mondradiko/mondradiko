/**
 * @file DisplayInterface.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for user-facing displays, VR or otherwise.
 * @date 2020-11-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <vector>

#include "core/displays/ViewportInterface.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declaration because of codependence
class GpuInstance;
class Renderer;

struct VulkanRequirements {
  uint32_t min_api_version;
  uint32_t max_api_version;

  std::vector<std::string> instance_extensions;
  std::vector<std::string> device_extensions;
};

struct DisplayPollEventsInfo {
  Renderer* renderer;
  bool should_run;
  bool should_quit;
};

struct DisplayBeginFrameInfo {
  double dt;
  bool should_render;
};

class DisplayInterface {
 public:
  virtual ~DisplayInterface() {}

  virtual bool getVulkanRequirements(VulkanRequirements*) = 0;
  virtual bool getVulkanDevice(VkInstance, VkPhysicalDevice*) = 0;
  virtual bool createSession(GpuInstance*) = 0;
  virtual void destroySession() = 0;

  virtual VkFormat getSwapchainFormat() = 0;
  virtual VkImageLayout getFinalLayout() = 0;

  virtual void pollEvents(DisplayPollEventsInfo*) = 0;
  virtual void beginFrame(DisplayBeginFrameInfo*) = 0;
  virtual void acquireViewports(std::vector<ViewportInterface*>*) = 0;
  virtual void endFrame(DisplayBeginFrameInfo*) = 0;

 private:
};

}  // namespace mondradiko
