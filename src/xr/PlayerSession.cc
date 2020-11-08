/**
 * @file PlayerSession.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Handles the lifetime of an OpenXR session, receives events, manages
 * viewports, synchronizes frames, and creates inputs.
 * @date 2020-10-24
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

#include "xr/PlayerSession.h"

#include "graphics/Renderer.h"
#include "log/log.h"

namespace mondradiko {

PlayerSession::PlayerSession(XrDisplay* display,
                             VulkanInstance* vulkan_instance)
    : display(display), vulkan_instance(vulkan_instance) {
  log_dbg("Creating OpenXR session.");

  XrGraphicsBindingVulkanKHR vulkanBindings{
      .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR,
      .instance = vulkan_instance->instance,
      .physicalDevice = vulkan_instance->physicalDevice,
      .device = vulkan_instance->device,
      .queueFamilyIndex = vulkan_instance->graphics_queue_family};

  XrSessionCreateInfo createInfo{.type = XR_TYPE_SESSION_CREATE_INFO,
                                 .next = &vulkanBindings,
                                 .systemId = display->system_id};

  if (xrCreateSession(display->instance, &createInfo, &session) != XR_SUCCESS) {
    log_ftl("Failed to create OpenXR session.");
  }

  XrReferenceSpaceCreateInfo stageSpaceCreateInfo{
      .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
      .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE,
      .poseInReferenceSpace = {.orientation = {0.0, 0.0, 0.0, 1.0},
                               .position = {0.0, 0.0, 0.0}}};

  if (xrCreateReferenceSpace(session, &stageSpaceCreateInfo, &stage_space) !=
      XR_SUCCESS) {
    log_ftl("Failed to create OpenXR stage reference space.");
  }
}

PlayerSession::~PlayerSession() {
  log_dbg("Destroying OpenXR session.");

  if (stage_space != XR_NULL_HANDLE) xrDestroySpace(stage_space);
  if (session != XR_NULL_HANDLE) xrDestroySession(session);
}

void PlayerSession::pollEvents(bool* should_run, bool* should_quit) {
  XrEventDataBuffer event{.type = XR_TYPE_EVENT_DATA_BUFFER};

  while (xrPollEvent(display->instance, &event) == XR_SUCCESS) {
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
            *should_run = true;

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
            *should_quit = true;
            *should_run = false;
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
        *should_quit = true;
        break;
      }

      default:
        break;
    }  // switch (event.type)

    event = {.type = XR_TYPE_EVENT_DATA_BUFFER};
  }
}

void PlayerSession::beginFrame(double* dt, bool* should_render) {
  current_frame_state = {.type = XR_TYPE_FRAME_STATE};

  xrWaitFrame(session, nullptr, &current_frame_state);

  // Convert nanoseconds to seconds
  *dt = current_frame_state.predictedDisplayPeriod / 1000000000.0;

  if (current_frame_state.shouldRender == XR_TRUE) {
    *should_render = true;
  } else {
    *should_render = false;
  }

  xrBeginFrame(session, nullptr);
}

void PlayerSession::endFrame(Renderer* renderer, bool didRender) {
  XrCompositionLayerBaseHeader* layer = nullptr;
  XrCompositionLayerProjection projectionLayer{
      .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION};

  std::vector<XrCompositionLayerProjectionView> projectionViews;
  if (didRender) {
    layer = reinterpret_cast<XrCompositionLayerBaseHeader*>(&projectionLayer);

    XrViewState viewState{.type = XR_TYPE_VIEW_STATE};

    XrViewLocateInfo locateInfo{
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        .displayTime = current_frame_state.predictedDisplayTime,
        .space = stage_space};

    uint32_t viewCount = views.size();
    xrLocateViews(session, &locateInfo, &viewState, viewCount, &viewCount,
                  views.data());
    projectionViews.resize(viewCount);

    renderer->finishRender(&views, &projectionViews);

    projectionLayer.space = stage_space;
    projectionLayer.viewCount = projectionViews.size();
    projectionLayer.views = projectionViews.data();
  }

  XrFrameEndInfo endInfo{
      .type = XR_TYPE_FRAME_END_INFO,
      .displayTime = current_frame_state.predictedDisplayTime,
      .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
      .layerCount = static_cast<uint32_t>((layer == nullptr) ? 0 : 1),
      .layers = &layer};

  xrEndFrame(session, &endInfo);
}

void PlayerSession::enumerateSwapchainFormats(std::vector<VkFormat>* formats) {
  uint32_t formatCount;
  xrEnumerateSwapchainFormats(session, 0, &formatCount, nullptr);
  formats->resize(formatCount);

  std::vector<int64_t> formatCodes(formatCount);
  xrEnumerateSwapchainFormats(session, formatCount, &formatCount,
                              formatCodes.data());

  for (uint32_t i = 0; i < formatCount; i++) {
    (*formats)[i] = (VkFormat)formatCodes[i];
  }
}

bool PlayerSession::createViewports(std::vector<Viewport*>* viewports,
                                    VkFormat format, VkRenderPass renderPass) {
  // TODO(marceline-cramer) findViewConfiguration()
  uint32_t viewportCount;
  xrEnumerateViewConfigurationViews(display->instance, display->system_id,
                                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                    0, &viewportCount, nullptr);
  std::vector<XrViewConfigurationView> viewConfigs(viewportCount);
  xrEnumerateViewConfigurationViews(display->instance, display->system_id,
                                    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                    viewportCount, &viewportCount,
                                    viewConfigs.data());

  viewports->resize(0);
  views.resize(viewportCount);

  for (uint32_t i = 0; i < viewportCount; i++) {
    Viewport* viewport =
        new Viewport(format, renderPass, &viewConfigs[i], this, vulkan_instance);
    viewports->push_back(viewport);
  }

  return true;
}

}  // namespace mondradiko
