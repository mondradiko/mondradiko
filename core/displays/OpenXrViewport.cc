// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/OpenXrViewport.h"

#include "core/displays/OpenXrDisplay.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"

namespace mondradiko {

static glm::mat4 createProjectionFromFOV(const XrFovf fov, const float near_z,
                                         const float far_z) {
  const float tan_left = tanf(fov.angleLeft);
  const float tan_right = tanf(fov.angleRight);

  const float tan_down = tanf(fov.angleDown);
  const float tan_up = tanf(fov.angleUp);

  const float tan_width = tan_right - tan_left;
  const float tan_height = tan_up - tan_down;

  const float a11 = 2 / tan_width;
  const float a22 = 2 / tan_height;

  const float a31 = (tan_right + tan_left) / tan_width;
  const float a32 = (tan_up + tan_down) / tan_height;
  const float a33 = -far_z / (far_z - near_z);

  const float a43 = -(far_z * near_z) / (far_z - near_z);

  const float mat[16] = {a11, 0,   0,   0,  0, -a22, 0,   0,
                         a31, a32, a33, -1, 0, 0,    a43, 0};

  return glm::make_mat4(mat);
}

OpenXrViewport::OpenXrViewport(GpuInstance* gpu, OpenXrDisplay* display,
                               Renderer* renderer,
                               XrViewConfigurationView* view_config)
    : Viewport(display, gpu, renderer),
      gpu(gpu),
      display(display),
      renderer(renderer) {
  log_zone;

  _image_width = view_config->recommendedImageRectWidth;
  _image_height = view_config->recommendedImageRectHeight;

  XrSwapchainCreateInfo swapchainCreateInfo{
      .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
      .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
      .format = static_cast<int64_t>(display->getSwapchainFormat()),
      .sampleCount = 1,
      .width = _image_width,
      .height = _image_height,
      .faceCount = 1,
      .arraySize = 1,
      .mipCount = 1};

  if (xrCreateSwapchain(display->session, &swapchainCreateInfo, &swapchain) !=
      XR_SUCCESS) {
    log_ftl("Failed to create OpenXR swapchain.");
  }

  uint32_t image_count;
  xrEnumerateSwapchainImages(swapchain, 0, &image_count, nullptr);
  std::vector<XrSwapchainImageVulkanKHR> vulkan_images(image_count);
  xrEnumerateSwapchainImages(
      swapchain, image_count, &image_count,
      reinterpret_cast<XrSwapchainImageBaseHeader*>(vulkan_images.data()));

  _images.resize(image_count);
  for (uint32_t i = 0; i < image_count; i++) {
    _images[i].image = vulkan_images[i].image;
  }

  _createImages();
}

OpenXrViewport::~OpenXrViewport() {
  log_zone;

  _destroyImages();

  if (swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}

void OpenXrViewport::writeUniform(ViewportUniform* uniform) {
  log_zone;

  glm::quat viewOrientation =
      glm::quat(-view.pose.orientation.w, view.pose.orientation.x,
                view.pose.orientation.y, view.pose.orientation.z);

  glm::vec3 viewPosition = glm::vec3(view.pose.position.x, view.pose.position.y,
                                     view.pose.position.z);

  // Leave the axes at the origin
  viewPosition -= glm::vec3(0.0, 0.0, 0.0);

  uniform->view = glm::translate(glm::mat4(viewOrientation), -viewPosition);
  uniform->projection = createProjectionFromFOV(view.fov, 0.001, 1000.0);
  uniform->position = viewPosition;
}

void OpenXrViewport::updateView(const XrView& new_view) { view = new_view; }

void OpenXrViewport::writeCompositionLayers(
    XrCompositionLayerProjectionView* projection_view) {
  projection_view->type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
  projection_view->pose = view.pose;
  projection_view->fov = view.fov;
  projection_view->subImage = {.swapchain = swapchain};
  projection_view->subImage.imageRect = {
      .offset = {0, 0},
      .extent = {static_cast<int32_t>(_image_width),
                 static_cast<int32_t>(_image_height)}};
}

VkSemaphore OpenXrViewport::_acquireImage(uint32_t* image_index) {
  log_zone;

  XrSwapchainImageAcquireInfo acquireInfo{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO, .next = nullptr};

  xrAcquireSwapchainImage(swapchain, &acquireInfo, image_index);

  XrSwapchainImageWaitInfo waitInfo{.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
                                    .timeout = XR_INFINITE_DURATION};

  xrWaitSwapchainImage(swapchain, &waitInfo);

  return VK_NULL_HANDLE;
}

void OpenXrViewport::_releaseImage(uint32_t, VkSemaphore) {
  log_zone;

  XrSwapchainImageReleaseInfo release_info{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};

  xrReleaseSwapchainImage(swapchain, &release_info);
}

}  // namespace mondradiko
