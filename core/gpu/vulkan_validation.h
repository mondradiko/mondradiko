// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallbackVulkan(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData) {
  LogLevel severity;

  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = LogLevel::Info;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = LogLevel::Warn;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      severity = LogLevel::Error;
      break;
  }

  log_at(severity, pCallbackData->pMessage);
  return VK_FALSE;
}

}  // namespace core
}  // namespace mondradiko
