/**
 * @file OpenXrViewport.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an OpenXR swapchain and view.
 * @date 2020-11-09
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/displays/OpenXrViewport.h"

#include "core/displays/OpenXrDisplay.h"
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
    : gpu(gpu), display(display), renderer(renderer) {
  log_zone;

  image_width = view_config->recommendedImageRectWidth;
  image_height = view_config->recommendedImageRectHeight;

  XrSwapchainCreateInfo swapchainCreateInfo{
      .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
      .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
      .format = static_cast<int64_t>(display->getSwapchainFormat()),
      .sampleCount = 1,
      .width = image_width,
      .height = image_height,
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

  images.resize(image_count);
  for (uint32_t i = 0; i < image_count; i++) {
    images[i].image = vulkan_images[i].image;

    VkImageViewCreateInfo viewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = images[i].image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = display->getSwapchainFormat(),
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    if (vkCreateImageView(gpu->device, &viewCreateInfo, nullptr,
                          &images[i].image_view) != VK_SUCCESS) {
      log_ftl("Failed to create OpenXR viewport image view.");
    }

    VkFramebufferCreateInfo framebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderer->composite_pass,
        .attachmentCount = 1,
        .pAttachments = &images[i].image_view,
        .width = image_width,
        .height = image_height,
        .layers = 1};

    if (vkCreateFramebuffer(gpu->device, &framebufferCreateInfo, nullptr,
                            &images[i].framebuffer) != VK_SUCCESS) {
      log_ftl("Failed to create OpenXR viewport framebuffer.");
    }
  }
}

OpenXrViewport::~OpenXrViewport() {
  log_zone;

  for (ViewportImage& image : images) {
    vkDestroyImageView(gpu->device, image.image_view, nullptr);
    vkDestroyFramebuffer(gpu->device, image.framebuffer, nullptr);
  }

  if (swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}

VkSemaphore OpenXrViewport::acquire() {
  log_zone;

  XrSwapchainImageAcquireInfo acquireInfo{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO, .next = nullptr};

  xrAcquireSwapchainImage(swapchain, &acquireInfo, &current_image_index);

  XrSwapchainImageWaitInfo waitInfo{.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
                                    .timeout = XR_INFINITE_DURATION};

  xrWaitSwapchainImage(swapchain, &waitInfo);

  return VK_NULL_HANDLE;
}

void OpenXrViewport::beginRenderPass(VkCommandBuffer command_buffer,
                                     VkRenderPass render_pass) {
  log_zone;

  std::array<VkClearValue, 1> clear_values;

  clear_values[0].color = {0.2, 0.0, 0.0, 1.0};

  VkRenderPassBeginInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = render_pass,
      .framebuffer = images[current_image_index].framebuffer,
      .renderArea = {.offset = {0, 0}, .extent = {image_width, image_height}},
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data()};

  vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{.x = 0,
                      .y = 0,
                      .width = static_cast<float>(image_width),
                      .height = static_cast<float>(image_height),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = {image_width, image_height}};

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void OpenXrViewport::writeUniform(ViewportUniform* uniform) {
  log_zone;

  glm::quat viewOrientation =
      glm::quat(view.pose.orientation.x, view.pose.orientation.y,
                view.pose.orientation.z, view.pose.orientation.w);

  glm::vec3 viewPosition = glm::vec3(view.pose.position.x, view.pose.position.y,
                                     view.pose.position.z);

  // HACK(marceline-cramer) Move objects at the origin into the center of
  // Monado's dummy HMD view
  viewPosition += glm::vec3(0.0, -2.0, 2.0);

  uniform->view = glm::translate(glm::mat4(viewOrientation), -viewPosition);
  uniform->projection = createProjectionFromFOV(view.fov, 0.001, 1000.0);
}

void OpenXrViewport::release(VkSemaphore) {
  log_zone;

  XrSwapchainImageReleaseInfo release_info{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};

  xrReleaseSwapchainImage(swapchain, &release_info);
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
      .extent = {static_cast<int32_t>(image_width),
                 static_cast<int32_t>(image_height)}};
}

}  // namespace mondradiko
