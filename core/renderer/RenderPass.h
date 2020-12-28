/**
 * @file RenderPass.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Common render pass structures.
 * @date 2020-12-19
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "core/common/vulkan_headers.h"
#include "core/world/World.h"

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

struct OverlayPassFrameData {
  GpuVector* debug_vertices = nullptr;
  GpuVector* debug_indices = nullptr;

  uint16_t index_count;
};

struct PipelinedFrameData {
  // TODO(marceline-cramer) Use command pool per frame, per thread
  VkCommandBuffer command_buffer;
  VkSemaphore on_render_finished;
  VkFence is_in_use;

  GpuDescriptorPool* descriptor_pool;
  GpuVector* viewports;

  MeshPassFrameData mesh_pass;
  OverlayPassFrameData overlay_pass;
};

}  // namespace mondradiko
