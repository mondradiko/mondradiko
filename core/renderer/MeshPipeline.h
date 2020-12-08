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

#include <map>
#include <set>
#include <string>

#include "assets/AssetHandle.h"
#include "assets/AssetPool.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "assets/TextureAsset.h"
#include "components/MeshRendererComponent.h"
#include "gpu/GpuDescriptorSet.h"
#include "gpu/GpuDescriptorSetLayout.h"
#include "gpu/GpuDescriptorPool.h"
#include "gpu/GpuInstance.h"
#include "common/api_headers.h"

namespace mondradiko {

class MeshPipeline {
 public:
  MeshPipeline(GpuInstance*, GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~MeshPipeline();

  void allocateDescriptors(entt::registry&, GpuDescriptorPool*);
  void render(entt::registry&, VkCommandBuffer, GpuDescriptorSet*, uint32_t);

  GpuDescriptorSetLayout* material_layout;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  GpuBuffer* material_buffer = nullptr;

 private:
  GpuInstance* gpu;

  // Non-persistent frame data
  std::map<AssetHandle<MaterialAsset>, GpuDescriptorSet*> frame_materials;
};

}  // namespace mondradiko
