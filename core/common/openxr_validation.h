// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/openxr_headers.h"
#include "log/log.h"

namespace mondradiko {

static XRAPI_ATTR XrBool32 XRAPI_CALL
debugCallbackOpenXR(XrDebugUtilsMessageSeverityFlagsEXT message_severity,
                    XrDebugUtilsMessageTypeFlagsEXT message_type,
                    const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData) {
  LogLevel severity;

  switch (message_severity) {
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = LogLevel::Info;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = LogLevel::Warn;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      severity = LogLevel::Error;
      break;
  }

  log_at(severity, pCallbackData->message);
  return VK_FALSE;
}

}  // namespace mondradiko
