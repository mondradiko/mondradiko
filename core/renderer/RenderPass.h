/**
 * @file RenderPass.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Common render pass structures.
 * @date 2020-12-19
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include <unordered_map>

#include "core/assets/Asset.h"
#include "core/components/Component.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations;
class GpuDescriptorPool;
class GpuDescriptorSet;
class GpuVector;

struct MeshPassFrameData {
  GpuVector* material_buffer = nullptr;
  GpuVector* mesh_buffer = nullptr;

  GpuDescriptorSet* material_descriptor;
  GpuDescriptorSet* mesh_descriptor;

  std::unordered_map<AssetId, uint32_t> materials;
  std::unordered_map<AssetId, GpuDescriptorSet*> textures;
  std::unordered_map<EntityId, uint32_t> meshes;
};

struct PipelinedFrameData {
  // TODO(marceline-cramer) Use command pool per frame, per thread
  VkCommandBuffer command_buffer;
  VkSemaphore on_render_finished;
  VkFence is_in_use;

  GpuDescriptorPool* descriptor_pool;
  GpuVector* viewports;

  MeshPassFrameData mesh_pass;
};

}  // namespace mondradiko
