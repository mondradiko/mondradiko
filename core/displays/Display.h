// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class Avatar;
class GpuInstance;
class Renderer;
class UserInterface;
class Viewport;
class World;

struct VulkanRequirements {
  uint32_t min_api_version;
  uint32_t max_api_version;

  types::vector<types::string> instance_extensions;
  types::vector<types::string> device_extensions;
};

class Display {
 public:
  virtual ~Display() {}

  struct PollEventsInfo {
    Renderer* renderer;
    bool should_run;
    bool should_quit;
  };

  struct BeginFrameInfo {
    double dt;
    bool should_render;
  };

  virtual bool getVulkanRequirements(VulkanRequirements*) = 0;
  virtual bool getVulkanDevice(VkInstance, VkPhysicalDevice*) = 0;
  virtual bool createSession(GpuInstance*) = 0;
  virtual void setUserInterface(UserInterface*) = 0;
  virtual const Avatar* getAvatar(World*) = 0;
  virtual void destroySession() = 0;

  virtual VkFormat getHdrFormat();
  virtual VkFormat getSwapchainFormat() = 0;
  virtual VkImageLayout getFinalLayout() = 0;
  virtual VkFormat getDepthFormat() = 0;

  virtual void pollEvents(PollEventsInfo*) = 0;
  virtual void beginFrame(BeginFrameInfo*) = 0;
  virtual void acquireViewports(types::vector<Viewport*>*) = 0;
  virtual void endFrame(BeginFrameInfo*) = 0;

 protected:
  GpuInstance* gpu;
};

}  // namespace core
}  // namespace mondradiko
