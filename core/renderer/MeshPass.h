// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "core/renderer/RenderPass.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class MeshRendererComponent;
class GpuBuffer;
class GpuDescriptorPool;
class GpuDescriptorSet;
class GpuDescriptorSetLayout;
class GpuInstance;
class GpuVector;

struct MeshUniform {
  glm::mat4 model;
  alignas(16) uint32_t light_count;
};

class MeshPass {
 public:
  static void initCVars(CVarScope*);

  MeshPass(GpuInstance*, GpuDescriptorSetLayout*, VkRenderPass, uint32_t);
  ~MeshPass();

  void createFrameData(MeshPassFrameData&);
  void destroyFrameData(MeshPassFrameData&);
  void allocateDescriptors(EntityRegistry&, MeshPassFrameData&, AssetPool*,
                           GpuDescriptorPool*);
  void render(EntityRegistry&, MeshPassFrameData&, AssetPool*, VkCommandBuffer,
              GpuDescriptorSet*, uint32_t);

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
