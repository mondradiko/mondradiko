// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/Display.h"

#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

VkFormat Display::getHdrFormat() {
  types::vector<VkFormat> hdr_candidates = {
      VK_FORMAT_R16G16B16_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT,
      VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT};

  VkFormat hdr_format;
  bool result = gpu->findSupportedFormat(
      &hdr_candidates, VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT, &hdr_format);

  if (!result) {
    log_ftl("Failed to find supported HDR format");
  }

  return hdr_format;
}

}  // namespace core
}  // namespace mondradiko
