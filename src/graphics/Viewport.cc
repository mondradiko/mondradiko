/**
 * @file Viewport.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to cameras, from OpenXR or otherwise.
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

#include "graphics/Viewport.h"

#include <array>

#include "graphics/VulkanInstance.h"
#include "log/log.h"
#include "xr/PlayerSession.h"

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

Viewport::Viewport(VkFormat format, VkRenderPass renderPass,
                   XrViewConfigurationView* viewConfig, PlayerSession* _session,
                   VulkanInstance* _vulkanInstance) {
  log_dbg("Creating viewport.");

  session = _session;
  vulkanInstance = _vulkanInstance;

  width = viewConfig->recommendedImageRectWidth;
  height = viewConfig->recommendedImageRectHeight;

  XrSwapchainCreateInfo swapchainCreateInfo{
      .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
      .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
      .format = (int64_t)format,
      .sampleCount = 1,
      .width = width,
      .height = height,
      .faceCount = 1,
      .arraySize = 1,
      .mipCount = 1};

  if (xrCreateSwapchain(session->session, &swapchainCreateInfo, &swapchain) !=
      XR_SUCCESS) {
    log_ftl("Failed to create swapchain.");
  }

  uint32_t imageCount;
  xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
  std::vector<XrSwapchainImageVulkanKHR> vulkanImages(imageCount);
  xrEnumerateSwapchainImages(
      swapchain, imageCount, &imageCount,
      reinterpret_cast<XrSwapchainImageBaseHeader*>(vulkanImages.data()));

  images.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; i++) {
    images[i].image = vulkanImages[i].image;

    VkImageViewCreateInfo viewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = images[i].image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    if (vkCreateImageView(vulkanInstance->device, &viewCreateInfo, nullptr,
                          &images[i].imageView) != VK_SUCCESS) {
      log_ftl("Failed to create viewport image view.");
    }

    VkFramebufferCreateInfo framebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = 1,
        .pAttachments = &images[i].imageView,
        .width = width,
        .height = height,
        .layers = 1};

    if (vkCreateFramebuffer(vulkanInstance->device, &framebufferCreateInfo,
                            nullptr, &images[i].framebuffer) != VK_SUCCESS) {
      log_ftl("Failed to create viewport framebuffer.");
    }
  }
}

Viewport::~Viewport() {
  log_dbg("Destroying viewport.");

  for (ViewportImage& image : images) {
    vkDestroyImageView(vulkanInstance->device, image.imageView, nullptr);
    vkDestroyFramebuffer(vulkanInstance->device, image.framebuffer, nullptr);
  }

  if (swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}

void Viewport::updateView(XrView view,
                          XrCompositionLayerProjectionView* projection,
                          ViewportUniform* uniform) {
  *projection = {.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
                 .pose = view.pose,
                 .fov = view.fov};
  projection->subImage = {.swapchain = swapchain};
  projection->subImage.imageRect = {
      .offset = {0, 0}, .extent = {(int32_t)width, (int32_t)height}};

  glm::quat viewOrientation =
      glm::quat(view.pose.orientation.w, view.pose.orientation.x,
                view.pose.orientation.y, view.pose.orientation.z);

  glm::vec3 viewPosition = glm::vec3(view.pose.position.x, view.pose.position.y,
                                     view.pose.position.z);

  uniform->view = glm::translate(glm::mat4(viewOrientation), -viewPosition);
  uniform->projection = createProjectionFromFOV(view.fov, 0.001, 1000.0);
}

void Viewport::acquireSwapchainImage() {
  XrSwapchainImageAcquireInfo acquireInfo{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO, .next = nullptr};

  xrAcquireSwapchainImage(swapchain, &acquireInfo, &currentIndex);

  XrSwapchainImageWaitInfo waitInfo{.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
                                    .timeout = XR_INFINITE_DURATION};

  xrWaitSwapchainImage(swapchain, &waitInfo);
}

void Viewport::beginRenderPass(VkCommandBuffer commandBuffer,
                               VkRenderPass renderPass) {
  std::array<VkClearValue, 1> clearValues;

  clearValues[0].color = {0.2, 0.0, 0.0, 1.0};

  VkRenderPassBeginInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = renderPass,
      .framebuffer = images[currentIndex].framebuffer,
      .renderArea = {.offset = {0, 0}, .extent = {width, height}},
      .clearValueCount = static_cast<uint32_t>(clearValues.size()),
      .pClearValues = clearValues.data()};

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void Viewport::setCommandViewport(VkCommandBuffer commandBuffer) {
  VkViewport viewport{.x = 0,
                      .y = 0,
                      .width = static_cast<float>(width),
                      .height = static_cast<float>(height),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = {width, height}};

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Viewport::releaseSwapchainImage() {
  XrSwapchainImageReleaseInfo releaseInfo{
      .type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO, .next = nullptr};

  xrReleaseSwapchainImage(swapchain, &releaseInfo);
}

}  // namespace mondradiko
