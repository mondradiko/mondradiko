// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/Display.h"

#include "core/cvars/CVarScope.h"
#include "core/cvars/EnumCVar.h"
#include "core/displays/SdlDisplay.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

void Display::initCVars(CVarScope* cvars) {
  CVarScope* display = cvars->addChild("displays");

  SdlDisplay::initCVars(display);

  display->addValue<EnumCVar>(
      "antialiasing",
      EnumCVar::EnumRange{"none", "MSAAx2", "MSAAx4", "MSAAx8"});
}

Display::Display(const CVarScope* parent_cvars)
    : cvars(parent_cvars->getChild("displays")) {}

VkSampleCountFlagBits Display::getSampleCount() {
  VkSampleCountFlags counts =
      gpu->physical_device_properties.limits.framebufferColorSampleCounts &
      gpu->physical_device_properties.limits.framebufferDepthSampleCounts;

  const auto& antialiasing = cvars->get<EnumCVar>("antialiasing");
  VkSampleCountFlagBits requested = VK_SAMPLE_COUNT_1_BIT;

  if (antialiasing.test("MSAAx2")) {
    requested = VK_SAMPLE_COUNT_2_BIT;
  } else if (antialiasing.test("MSAAx4")) {
    requested = VK_SAMPLE_COUNT_4_BIT;
  } else if (antialiasing.test("MSAAx8")) {
    requested = VK_SAMPLE_COUNT_8_BIT;
  }

  if (!(counts & requested)) {
    log_err_fmt("Unsupported MSAA method %s", antialiasing.c_str());
    requested = VK_SAMPLE_COUNT_1_BIT;
  }

  return requested;
}

VkFormat Display::getHdrFormat() {
  if (gpu == nullptr) {
    log_ftl("Display::gpu has not been initialized");
  }

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
