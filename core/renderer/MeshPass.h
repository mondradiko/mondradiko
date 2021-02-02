// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <vector>

#include "core/assets/AssetHandle.h"
#include "core/assets/AssetPool.h"
#include "core/assets/MeshAsset.h"
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
class GpuPipeline;
class GpuShader;
class GpuVector;
class Renderer;
class World;

struct MeshUniform {
  glm::mat4 model;
  alignas(16) uint32_t light_count;
};

class MeshPass : public RenderPass {
 public:
  static void initCVars(CVarScope*);

  MeshPass(Renderer*, World*);
  ~MeshPass();

  // RenderPass implementation
  void createFrameData(uint32_t) final;
  void destroyFrameData() final;
  void allocateDescriptors(uint32_t, GpuDescriptorPool*) final;
  void preRender(uint32_t, VkCommandBuffer) final {}
  void render(uint32_t, VkCommandBuffer, const GpuDescriptorSet*) final;

 private:
  GpuInstance* gpu;
  Renderer* renderer;
  World* world;

  GpuShader* vertex_shader = nullptr;
  GpuShader* fragment_shader = nullptr;

  GpuDescriptorSetLayout* material_layout;
  GpuDescriptorSetLayout* texture_layout;
  GpuDescriptorSetLayout* mesh_layout;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  GpuPipeline* pipeline = nullptr;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  struct MeshRenderCommand {
    uint32_t mesh_idx;
    uint32_t material_idx;
    GpuDescriptorSet* textures_descriptor;

    AssetHandle<MeshAsset> mesh_asset;
  };

  struct FrameData {
    GpuVector* material_buffer = nullptr;
    GpuVector* mesh_buffer = nullptr;
    GpuVector* point_lights = nullptr;

    GpuDescriptorSet* material_descriptor;
    GpuDescriptorSet* mesh_descriptor;

    std::vector<MeshRenderCommand> commands;
  };

  std::vector<FrameData> frame_data;
};

}  // namespace mondradiko
