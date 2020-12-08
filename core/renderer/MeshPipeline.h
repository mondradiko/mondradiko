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

#include "core/assets/AssetHandle.h"
#include "core/assets/AssetPool.h"
#include "core/assets/MaterialAsset.h"
#include "core/assets/MeshAsset.h"
#include "core/assets/TextureAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuInstance.h"
#include "core/common/api_headers.h"

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
