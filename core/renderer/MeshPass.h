// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetHandle.h"
#include "core/assets/AssetPool.h"
#include "core/assets/MeshAsset.h"
#include "core/renderer/RenderPass.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

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
  uint32_t light_count;
  uint32_t material_idx;

  // Padding for buffer alignment
  uint32_t pad1;
  uint32_t pad2;
};

class MeshPass : public RenderPass {
 public:
  static void initCVars(CVarScope*);
  static void initDummyAssets(AssetPool*);

  MeshPass(Renderer*, World*);
  ~MeshPass();

  Renderer* getRenderer() { return renderer; }

  size_t allocateVertices(size_t);
  size_t allocateIndices(size_t);

  GpuBuffer* getVertexPool() { return vertex_pool; }
  GpuBuffer* getIndexPool() { return index_pool; }

  // RenderPass implementation
  void createFrameData(uint32_t) final;
  void destroyFrameData() final;

  void beginFrame(uint32_t, GpuDescriptorPool*) final;
  void render(RenderPhase, VkCommandBuffer) final {}
  void renderViewport(RenderPhase, VkCommandBuffer,
                      const GpuDescriptorSet*) final;
  void endFrame() final {}

 private:
  GpuInstance* gpu;
  Renderer* renderer;
  World* world;

  GpuShader* depth_vertex_shader = nullptr;
  GpuShader* depth_fragment_shader = nullptr;
  GpuShader* forward_vertex_shader = nullptr;
  GpuShader* forward_fragment_shader = nullptr;

  GpuDescriptorSetLayout* material_layout;
  GpuDescriptorSetLayout* texture_layout;
  GpuDescriptorSetLayout* mesh_layout;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  GpuPipeline* depth_pipeline = nullptr;
  GpuPipeline* forward_pipeline = nullptr;
  GpuPipeline* transparent_pipeline = nullptr;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  GpuBuffer* vertex_pool = nullptr;
  GpuBuffer* index_pool = nullptr;
  size_t first_available_vertex = 0;
  size_t first_available_index = 0;

  struct MeshRenderCommand {
    uint32_t mesh_idx;
    GpuDescriptorSet* textures_descriptor;

    uint32_t vertex_offset;
    uint32_t index_offset;
    uint32_t index_num;
  };

  // Helper function to render meshes
  using MeshRenderCommandList = types::vector<MeshRenderCommand>;
  void executeMeshCommands(VkCommandBuffer, const MeshRenderCommandList&);

  struct MeshPassCommandList {
    MeshRenderCommandList single_sided;
    MeshRenderCommandList double_sided;
  };

  struct FrameData {
    GpuVector* material_buffer = nullptr;
    GpuVector* mesh_buffer = nullptr;
    GpuVector* point_lights = nullptr;

    GpuDescriptorSet* material_descriptor;
    GpuDescriptorSet* mesh_descriptor;

    MeshPassCommandList forward_commands;
    MeshPassCommandList transparent_commands;
  };

  types::vector<FrameData> frame_data;
  uint32_t current_frame;
};

}  // namespace core
}  // namespace mondradiko
