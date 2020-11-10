/**
 * @file OpenXrDisplay.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements an OpenXR VR display interface.
 * @date 2020-11-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "displays/OpenXrDisplay.h"

#include "./build_config.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"

#define XR_LOAD_FN_PTR(name, fnPtr)     \
  xrGetInstanceProcAddr(instance, name, \
                        reinterpret_cast<PFN_xrVoidFunction*>(&fnPtr))

namespace mondradiko {

static XRAPI_ATTR XrBool32 XRAPI_CALL
debugCallback(XrDebugUtilsMessageSeverityFlagsEXT message_severity,
              XrDebugUtilsMessageTypeFlagsEXT message_type,
              const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  LogLevel severity;

  switch (message_severity) {
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = LOG_LEVEL_INFO;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = LOG_LEVEL_WARNING;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      severity = LOG_LEVEL_ERROR;
      break;
  }

  log("OpenXRValidation", 0, severity, pCallbackData->message);
  return VK_FALSE;
}

void splitString(std::vector<std::string>* split, const std::string& source) {
  split->clear();
  uint32_t start_index = 0;
  for (uint32_t end_index = 0; end_index <= source.size(); end_index++) {
    if (source[end_index] == ' ' || source[end_index] == '\0') {
      if (end_index <= start_index) continue;

      std::string token = source.substr(start_index, end_index - start_index);
      split->push_back(token);
      start_index = end_index + 1;
    }
  }
}

OpenXrDisplay::OpenXrDisplay() {
  createInstance();

  if (enable_validation_layers) {
    setupDebugMessenger();
  }

  findSystem();
}

OpenXrDisplay::~OpenXrDisplay() {
  log_dbg("Destroying OpenXR display.");

  destroySession();

  if (enable_validation_layers && debug_messenger != VK_NULL_HANDLE)
    ext_xrDestroyDebugUtilsMessengerEXT(debug_messenger);

  if (instance != XR_NULL_HANDLE) xrDestroyInstance(instance);
}

bool OpenXrDisplay::createSession(VulkanInstance* _vulkan_instance) {
  log_dbg("Creating OpenXR session.");

  vulkan_instance = _vulkan_instance;

  XrGraphicsBindingVulkanKHR vulkanBindings{
      .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR,
      .instance = vulkan_instance->instance,
      .physicalDevice = vulkan_instance->physical_device,
      .device = vulkan_instance->device,
      .queueFamilyIndex = vulkan_instance->graphics_queue_family};

  XrSessionCreateInfo createInfo{.type = XR_TYPE_SESSION_CREATE_INFO,
                                 .next = &vulkanBindings,
                                 .systemId = system_id};

  if (xrCreateSession(instance, &createInfo, &session) != XR_SUCCESS) {
    log_err("Failed to create OpenXR session.");
    return false;
  }

  XrReferenceSpaceCreateInfo stageSpaceCreateInfo{
      .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
      .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE,
      .poseInReferenceSpace = {.orientation = {0.0, 0.0, 0.0, 1.0},
                               .position = {0.0, 0.0, 0.0}}};

  if (xrCreateReferenceSpace(session, &stageSpaceCreateInfo, &stage_space) !=
      XR_SUCCESS) {
    log_err("Failed to create OpenXR stage reference space.");
    return false;
  }

  uint32_t format_count;
  xrEnumerateSwapchainFormats(session, 0, &format_count, nullptr);
  std::vector<int64_t> format_codes(format_count);
  xrEnumerateSwapchainFormats(session, format_count, &format_count,
                              format_codes.data());

  std::vector<VkFormat> format_options(format_count);
  for (uint32_t i = 0; i < format_count; i++) {
    format_options[i] = static_cast<VkFormat>(format_codes[i]);
  }

  std::vector<VkFormat> format_candidates = {VK_FORMAT_R8G8B8A8_SRGB,
                                             VK_FORMAT_R8G8B8A8_UNORM};

  if (!vulkan_instance->findFormatFromOptions(
          &format_options, &format_candidates, &swapchain_format)) {
    log_ftl("Failed to find suitable swapchain format.");
  }

  return true;
}

void OpenXrDisplay::destroySession() {
  for (auto& viewport : viewports) delete viewport;
  if (stage_space != XR_NULL_HANDLE) xrDestroySpace(stage_space);
  if (session != XR_NULL_HANDLE) xrDestroySession(session);

  viewports.resize(0);
  stage_space = XR_NULL_HANDLE;
  session = XR_NULL_HANDLE;
}

bool OpenXrDisplay::getRequirements(VulkanRequirements* requirements) {
  XrGraphicsRequirementsVulkanKHR vulkanRequirements{
      .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};

  if (ext_xrGetVulkanGraphicsRequirementsKHR(
          instance, system_id, &vulkanRequirements) != XR_SUCCESS) {
    log_err("Failed to get OpenXR Vulkan requirements.");
    return false;
  }

  requirements->min_api_version = vulkanRequirements.minApiVersionSupported;
  requirements->max_api_version = vulkanRequirements.maxApiVersionSupported;

  uint32_t instance_extensions_length;
  ext_xrGetVulkanInstanceExtensionsKHR(instance, system_id, 0,
                                       &instance_extensions_length, nullptr);
  std::vector<char> instance_extensions_list(instance_extensions_length);
  ext_xrGetVulkanInstanceExtensionsKHR(
      instance, system_id, instance_extensions_length,
      &instance_extensions_length, instance_extensions_list.data());
  std::string instance_extensions_names = instance_extensions_list.data();
  splitString(&requirements->instance_extensions, instance_extensions_names);

  uint32_t device_extensions_length;
  ext_xrGetVulkanDeviceExtensionsKHR(instance, system_id, 0,
                                     &device_extensions_length, nullptr);
  std::vector<char> device_extensions_list(device_extensions_length);
  ext_xrGetVulkanDeviceExtensionsKHR(
      instance, system_id, device_extensions_length, &device_extensions_length,
      device_extensions_list.data());
  std::string device_extensions_names = device_extensions_list.data();
  splitString(&requirements->device_extensions, device_extensions_names);

  return true;
}

bool OpenXrDisplay::getVulkanDevice(VkInstance vk_instance,
                                    VkPhysicalDevice* vk_physical_device) {
  if (ext_xrGetVulkanGraphicsDeviceKHR(instance, system_id, vk_instance,
                                       vk_physical_device) != XR_SUCCESS) {
    log_err("Failed to get Vulkan physical device.");
    return false;
  }

  return true;
}

void OpenXrDisplay::pollEvents(DisplayPollEventsInfo* poll_info) {
  XrEventDataBuffer event{.type = XR_TYPE_EVENT_DATA_BUFFER};

  while (xrPollEvent(instance, &event) == XR_SUCCESS) {
    switch (event.type) {
      // Handle session state change events
      // Quitting, app focus, ready, etc.
      case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
        XrEventDataSessionStateChanged* changed =
            reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
        session_state = changed->state;

        switch (session_state) {
          case XR_SESSION_STATE_READY: {
            log_dbg("OpenXR session ready; beginning session.");

            XrSessionBeginInfo beginInfo{
                .type = XR_TYPE_SESSION_BEGIN_INFO,
                .primaryViewConfigurationType =
                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
            xrBeginSession(session, &beginInfo);
            createViewports(poll_info->renderer);

            break;
          }

          case XR_SESSION_STATE_VISIBLE: {
            log_dbg("OpenXR session is visible.");
            break;
          }

          case XR_SESSION_STATE_FOCUSED: {
            log_dbg("OpenXR session is focused.");
            break;
          }

          case XR_SESSION_STATE_IDLE: {
            log_dbg("OpenXR session is idle.");
            break;
          }

          case XR_SESSION_STATE_STOPPING:
          case XR_SESSION_STATE_EXITING:
          case XR_SESSION_STATE_LOSS_PENDING: {
            log_dbg("Ending OpenXR session.");
            xrEndSession(session);
            break;
          }

          default:
            break;
        }  // switch (session_state)

        break;
      }
      // If the instance is about to be lost,
      // just exit.
      case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
        poll_info->should_quit = true;
        return;
      }

      default:
        break;
    }  // switch (event.type)

    event = {.type = XR_TYPE_EVENT_DATA_BUFFER};
  }

  switch (session_state) {
    case XR_SESSION_STATE_UNKNOWN:
    case XR_SESSION_STATE_IDLE:
    default: {
      poll_info->should_quit = false;
      poll_info->should_run = false;
      break;
    }

    case XR_SESSION_STATE_READY:
    case XR_SESSION_STATE_VISIBLE:
    case XR_SESSION_STATE_FOCUSED: {
      poll_info->should_quit = false;
      poll_info->should_run = true;
      break;
    }

    case XR_SESSION_STATE_STOPPING:
    case XR_SESSION_STATE_LOSS_PENDING:
    case XR_SESSION_STATE_EXITING: {
      poll_info->should_quit = true;
      poll_info->should_run = false;
    }
  }
}

void OpenXrDisplay::beginFrame(DisplayBeginFrameInfo* frame_info) {
  current_frame_state = {.type = XR_TYPE_FRAME_STATE};

  xrWaitFrame(session, nullptr, &current_frame_state);

  // Convert nanoseconds to seconds
  frame_info->dt = current_frame_state.predictedDisplayPeriod / 1000000000.0;

  if (current_frame_state.shouldRender == XR_TRUE) {
    frame_info->should_render = true;
  } else {
    frame_info->should_render = false;
  }

  xrBeginFrame(session, nullptr);
}

void OpenXrDisplay::acquireViewports(
    std::vector<ViewportInterface*>* acquired) {
  acquired->resize(viewports.size());

  XrViewState view_state{.type = XR_TYPE_VIEW_STATE};

  XrViewLocateInfo locate_info{
      .type = XR_TYPE_VIEW_LOCATE_INFO,
      .viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
      .displayTime = current_frame_state.predictedDisplayTime,
      .space = stage_space};

  std::vector<XrView> views(viewports.size());
  uint32_t view_count = viewports.size();
  xrLocateViews(session, &locate_info, &view_state, view_count, &view_count,
                views.data());

  for (uint32_t i = 0; i < acquired->size(); i++) {
    viewports[i]->acquireSwapchainImage();
    acquired->at(i) = viewports.at(i);
    viewports[i]->updateView(views[i]);
  }
}

void OpenXrDisplay::endFrame(DisplayBeginFrameInfo* frame_info) {
  XrCompositionLayerBaseHeader* layer = nullptr;
  XrCompositionLayerProjection projection_layer{
      .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION};
  std::vector<XrCompositionLayerProjectionView> projection_views;

  if (frame_info->should_render) {
    layer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&projection_layer);

    projection_views.resize(viewports.size());
    for (uint32_t i = 0; i < viewports.size(); i++) {
      viewports[i]->writeCompositionLayers(&projection_views[i]);
    }

    projection_layer.space = stage_space;
    projection_layer.viewCount = static_cast<uint32_t>(projection_views.size());
    projection_layer.views = projection_views.data();
  }

  XrFrameEndInfo endInfo{
      .type = XR_TYPE_FRAME_END_INFO,
      .displayTime = current_frame_state.predictedDisplayTime,
      .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
      .layerCount = static_cast<uint32_t>((layer == nullptr) ? 0 : 1),
      .layers = &layer};

  xrEndFrame(session, &endInfo);
}

void OpenXrDisplay::populateDebugMessengerCreateInfo(
    XrDebugUtilsMessengerCreateInfoEXT* messenger_info) {
  *messenger_info = {
      .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT,
      .userCallback = debugCallback};
}

void OpenXrDisplay::createInstance() {
  log_dbg("Creating OpenXR instance.");

  XrApplicationInfo appInfo{.applicationVersion = XR_MAKE_VERSION(0, 0, 0),
                            .engineVersion = MONDRADIKO_OPENXR_VERSION,
                            .apiVersion = XR_MAKE_VERSION(1, 0, 0)};

  snprintf(appInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE,
           "Mondradiko Client");
  snprintf(appInfo.engineName, XR_MAX_ENGINE_NAME_SIZE, MONDRADIKO_NAME);

  std::vector<const char*> extensions{XR_KHR_VULKAN_ENABLE_EXTENSION_NAME,
                                      XR_EXT_DEBUG_UTILS_EXTENSION_NAME};

  // TODO(marceline-cramer) Validation layers
  XrInstanceCreateInfo instance_info{
      .type = XR_TYPE_INSTANCE_CREATE_INFO,
      .applicationInfo = appInfo,
      .enabledApiLayerCount = 0,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .enabledExtensionNames = extensions.data()};

  XrResult result = xrCreateInstance(&instance_info, &instance);

  if (result != XR_SUCCESS || instance == nullptr) {
    log_ftl("Failed to create OpenXR instance. Is an OpenXR runtime running?");
  }

  XR_LOAD_FN_PTR("xrCreateDebugUtilsMessengerEXT",
                 ext_xrCreateDebugUtilsMessengerEXT);

  XR_LOAD_FN_PTR("xrDestroyDebugUtilsMessengerEXT",
                 ext_xrDestroyDebugUtilsMessengerEXT);

  XR_LOAD_FN_PTR("xrGetVulkanGraphicsRequirementsKHR",
                 ext_xrGetVulkanGraphicsRequirementsKHR);

  XR_LOAD_FN_PTR("xrGetVulkanInstanceExtensionsKHR",
                 ext_xrGetVulkanInstanceExtensionsKHR);

  XR_LOAD_FN_PTR("xrGetVulkanGraphicsDeviceKHR",
                 ext_xrGetVulkanGraphicsDeviceKHR);

  XR_LOAD_FN_PTR("xrGetVulkanDeviceExtensionsKHR",
                 ext_xrGetVulkanDeviceExtensionsKHR);
}

void OpenXrDisplay::setupDebugMessenger() {
  XrDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(&messenger_info);

  if (ext_xrCreateDebugUtilsMessengerEXT(instance, &messenger_info,
                                         &debug_messenger) != XR_SUCCESS) {
    log_ftl("Failed to create OpenXR debug messenger.");
  }
}

void OpenXrDisplay::findSystem() {
  log_dbg("Choosing OpenXR system.");

  XrSystemGetInfo systemInfo{.type = XR_TYPE_SYSTEM_GET_INFO,
                             .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};

  if (xrGetSystem(instance, &systemInfo, &system_id) != XR_SUCCESS) {
    log_ftl("Failed to find OpenXR HMD.");
  }
}

void OpenXrDisplay::createViewports(Renderer* renderer) {
  uint32_t viewport_count;
  xrEnumerateViewConfigurationViews(instance, system_id,
                                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                    0, &viewport_count, nullptr);
  std::vector<XrViewConfigurationView> view_configurations(viewport_count);
  xrEnumerateViewConfigurationViews(
      instance, system_id, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
      viewport_count, &viewport_count, view_configurations.data());

  for (uint32_t i = 0; i < viewport_count; i++) {
    OpenXrViewport* viewport = new OpenXrViewport(
        this, renderer, vulkan_instance, &view_configurations[i]);
    viewports.push_back(viewport);
  }
}

}  // namespace mondradiko
