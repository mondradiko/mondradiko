// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"

namespace mondradiko {

// Forward declarations;
class GpuDescriptorPool;
class GpuDescriptorSet;

class RenderPass {
 public:
  virtual ~RenderPass() {}

  virtual void createFrameData(uint32_t) = 0;
  virtual void destroyFrameData() = 0;
  virtual void allocateDescriptors(uint32_t, GpuDescriptorPool*) = 0;
  virtual void render(uint32_t, VkCommandBuffer, const GpuDescriptorSet*) = 0;

 private:
};

}  // namespace mondradiko
