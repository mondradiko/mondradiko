/**
 * @file MeshPass.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "core/common/glm_headers.h"
#include "core/renderer/RenderPass.h"

namespace mondradiko {

// Forward declarations
class MeshRendererComponent;
class GpuBuffer;
class GpuDescriptorPool;
class GpuDescriptorSet;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;

struct MeshUniform {
  glm::mat4 model;
};

class MeshPass {
 public:
  MeshPass(GpuInstance*, GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~MeshPass();

  void createFrameData(MeshPassFrameData&);
  void destroyFrameData(MeshPassFrameData&);
  void allocateDescriptors(EntityRegistry&, MeshPassFrameData&,
                           const AssetPool*, GpuDescriptorPool*);
  void render(EntityRegistry&, MeshPassFrameData&, const AssetPool*,
              VkCommandBuffer, GpuDescriptorSet*, uint32_t);

  GpuDescriptorSetLayout* material_layout;
  GpuDescriptorSetLayout* texture_layout;
  GpuDescriptorSetLayout* mesh_layout;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkSampler texture_sampler = VK_NULL_HANDLE;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko
