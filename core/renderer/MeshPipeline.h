/**
 * @file MeshPipeline.h
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
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class MeshRendererComponent;
class GpuBuffer;
class GpuDescriptorPool;
class GpuDescriptorSet;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;

class MeshPipeline {
 public:
  MeshPipeline(GpuInstance*, GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~MeshPipeline();

  void allocateDescriptors(entt::registry&, const AssetPool*,
                           GpuDescriptorPool*);
  void render(entt::registry&, const AssetPool*, VkCommandBuffer,
              GpuDescriptorSet*, uint32_t);

  GpuDescriptorSetLayout* material_layout;
  GpuDescriptorSetLayout* texture_layout;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  GpuVector* material_buffer = nullptr;

 private:
  GpuInstance* gpu;

  // Non-persistent frame data
  GpuDescriptorSet* material_descriptor;
  std::unordered_map<AssetId, uint32_t> frame_materials;
  std::unordered_map<AssetId, GpuDescriptorSet*> frame_textures;
};

}  // namespace mondradiko
