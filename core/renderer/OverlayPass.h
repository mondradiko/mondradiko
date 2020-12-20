/**
 * @file OverlayPass.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Render pass for UI and debug draw.
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/renderer/RenderPass.h"

namespace mondradiko {

// Forward declarations
class GpuInstance;

class OverlayPass {
 public:
  OverlayPass(GpuInstance*);
  ~OverlayPass();

  void createFrameData(OverlayPassFrameData&);
  void destroyFrameData(OverlayPassFrameData&);
  void allocateDescriptors(entt::registry&, OverlayPassFrameData&,
                           const AssetPool*, GpuDescriptorPool*);
  void render(entt::registry&, OverlayPassFrameData&, const AssetPool*,
              VkCommandBuffer, GpuDescriptorSet*, uint32_t);

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko