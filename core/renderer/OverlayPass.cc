/**
 * @file OverlayPass.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Render pass for UI and debug draw.
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/renderer/OverlayPass.h"

namespace mondradiko {

OverlayPass::OverlayPass(GpuInstance* gpu) : gpu(gpu) {}

OverlayPass::~OverlayPass() {}

void OverlayPass::createFrameData(OverlayPassFrameData&) {}

void OverlayPass::destroyFrameData(OverlayPassFrameData&) {}

void OverlayPass::allocateDescriptors(entt::registry&, OverlayPassFrameData&,
                                      const AssetPool*, GpuDescriptorPool*) {}

void OverlayPass::render(entt::registry&, OverlayPassFrameData&,
                         const AssetPool*, VkCommandBuffer, GpuDescriptorSet*,
                         uint32_t) {}

}  // namespace mondradiko
