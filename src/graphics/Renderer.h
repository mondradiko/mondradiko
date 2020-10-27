/**
 * @file Renderer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all Pipelines and performs frame rendering operations.
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

#ifndef SRC_GRAPHICS_RENDERER_H_
#define SRC_GRAPHICS_RENDERER_H_

#include <vector>

#include "graphics/CameraDescriptorSet.h"
#include "graphics/FrameData.h"
#include "graphics/Viewport.h"
#include "graphics/VulkanInstance.h"
#include "graphics/pipelines/MeshPipeline.h"
#include "src/api_headers.h"
#include "xr/PlayerSession.h"

class Renderer {
 public:
  Renderer(VulkanInstance*, PlayerSession*);
  ~Renderer();

  void renderFrame();
  void finishRender(std::vector<XrView>*,
                    std::vector<XrCompositionLayerProjectionView>*);

  VkFormat swapchainFormat;
  VkRenderPass compositePass = VK_NULL_HANDLE;
  std::vector<Viewport*> viewports;

  FrameData* frameData = nullptr;
  MeshPipeline* meshPipeline = nullptr;
  CameraDescriptorSet* cameraDescriptorSet = nullptr;

 private:
  VulkanInstance* vulkanInstance;
  PlayerSession* session;

  void findSwapchainFormat();
  void createRenderPasses();
  void createPipelines();
};

#endif  // SRC_GRAPHICS_RENDERER_H_
