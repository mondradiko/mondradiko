// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/MeshPass.h"

#include <vector>

#include "core/assets/MeshAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/components/PointLightComponent.h"
#include "core/components/TransformComponent.h"
#include "core/cvars/CVarScope.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuPipeline.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GpuVector.h"
#include "core/gpu/GraphicsState.h"
#include "core/renderer/Renderer.h"
#include "core/world/World.h"
#include "log/log.h"
#include "shaders/mesh.frag.h"
#include "shaders/mesh.vert.h"

namespace mondradiko {

void MeshPass::initCVars(CVarScope* cvars) {}

MeshPass::MeshPass(GpuInstance* gpu, World* world,
                   GpuDescriptorSetLayout* viewport_layout,
                   VkRenderPass render_pass, uint32_t subpass_index)
    : gpu(gpu), world(world) {
  log_zone;

  {
    log_zone_named("Create texture sampler");

    VkSamplerCreateInfo sampler_info{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        // TODO(marceline-cramer) Anisotropy support
        .anisotropyEnable = VK_FALSE,
        .compareEnable = VK_FALSE,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE};

    if (vkCreateSampler(gpu->device, &sampler_info, nullptr,
                        &texture_sampler) != VK_SUCCESS) {
      log_ftl("Failed to create texture sampler.");
    }
  }

  {
    log_zone_named("Create set layouts");

    material_layout = new GpuDescriptorSetLayout(gpu);
    material_layout->addDynamicUniformBuffer(sizeof(MaterialUniform));

    texture_layout = new GpuDescriptorSetLayout(gpu);
    texture_layout->addCombinedImageSampler(texture_sampler);

    mesh_layout = new GpuDescriptorSetLayout(gpu);
    mesh_layout->addDynamicUniformBuffer(sizeof(MeshUniform));
    mesh_layout->addStorageBuffer(sizeof(PointLightUniform));
  }

  {
    log_zone_named("Create pipeline layout");

    std::vector<VkDescriptorSetLayout> set_layouts{
        viewport_layout->getSetLayout(), material_layout->getSetLayout(),
        texture_layout->getSetLayout(), mesh_layout->getSetLayout()};

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(set_layouts.size()),
        .pSetLayouts = set_layouts.data()};

    if (vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr,
                               &pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout.");
    }
  }

  {
    log_zone_named("Create shaders");

    vertex_shader = new GpuShader(gpu, VK_SHADER_STAGE_VERTEX_BIT,
                                  shaders_mesh_vert, sizeof(shaders_mesh_vert));
    fragment_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_mesh_frag,
                      sizeof(shaders_mesh_frag));
  }

  {
    log_zone_named("Create pipeline");

    auto vertex_bindings = MeshVertex::getVertexBindings();
    auto attribute_descriptions = MeshVertex::getAttributeDescriptions();

    pipeline = new GpuPipeline(gpu, pipeline_layout, render_pass, subpass_index,
                               vertex_shader, fragment_shader, vertex_bindings,
                               attribute_descriptions);
  }
}

MeshPass::~MeshPass() {
  log_zone;

  if (texture_sampler != VK_NULL_HANDLE)
    vkDestroySampler(gpu->device, texture_sampler, nullptr);
  if (pipeline != nullptr) delete pipeline;
  if (vertex_shader != nullptr) delete vertex_shader;
  if (fragment_shader != nullptr) delete fragment_shader;
  if (pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, pipeline_layout, nullptr);
  if (material_layout != nullptr) delete material_layout;
  if (texture_layout != nullptr) delete texture_layout;
  if (mesh_layout != nullptr) delete mesh_layout;
}

void MeshPass::createFrameData(uint32_t frame_count) {
  log_zone;

  frame_data.resize(frame_count);

  for (auto& frame : frame_data) {
    frame.material_buffer = new GpuVector(gpu, sizeof(MaterialUniform),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    frame.mesh_buffer = new GpuVector(gpu, sizeof(MeshUniform),
                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    frame.point_lights = new GpuVector(gpu, sizeof(PointLightUniform),
                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  }
}

void MeshPass::destroyFrameData() {
  log_zone;

  for (auto& frame : frame_data) {
    if (frame.material_buffer != nullptr) delete frame.material_buffer;
    if (frame.mesh_buffer != nullptr) delete frame.mesh_buffer;
    if (frame.point_lights != nullptr) delete frame.point_lights;
  }
}

void MeshPass::allocateDescriptors(uint32_t frame_index,
                                   GpuDescriptorPool* descriptor_pool) {
  log_zone;

  auto& frame = frame_data[frame_index];

  uint32_t light_count;

  {
    auto point_lights = world->registry.view<PointLightComponent>();
    std::vector<PointLightUniform> point_light_uniforms;

    for (auto e : point_lights) {
      auto& point_light = point_lights.get(e);

      PointLightUniform uniform;
      point_light.getUniform(&uniform);
      point_light_uniforms.push_back(uniform);
    }

    light_count = point_light_uniforms.size();

    for (uint32_t i = 0; i < light_count; i++) {
      frame.point_lights->writeElement(i, point_light_uniforms[i]);
    }
  }

  std::unordered_map<AssetId, uint32_t> material_assets;
  std::vector<MaterialUniform> frame_materials;
  std::vector<GpuDescriptorSet*> frame_textures;

  std::vector<MeshUniform> frame_meshes;

  frame.commands.clear();

  auto mesh_renderers =
      world->registry.view<MeshRendererComponent, TransformComponent>();

  for (auto e : mesh_renderers) {
    auto& mesh_renderer = mesh_renderers.get<MeshRendererComponent>(e);
    if (!mesh_renderer.isLoaded()) continue;

    MeshRenderCommand cmd;

    {  // Write material uniform
      const auto& material_asset = mesh_renderer.getMaterialAsset();
      const auto iter = material_assets.find(material_asset.getId());

      if (iter != material_assets.end()) {
        cmd.material_idx = iter->second;
        cmd.textures_descriptor = frame_textures[iter->second];
      } else {
        cmd.material_idx = frame_materials.size();
        material_assets.emplace(material_asset.getId(), cmd.material_idx);
        frame_materials.push_back(material_asset->getUniform());

        cmd.textures_descriptor = descriptor_pool->allocate(texture_layout);
        material_asset->updateTextureDescriptor(cmd.textures_descriptor);
        frame_textures.push_back(cmd.textures_descriptor);
      }
    }

    {  // Write mesh uniform
      auto& transform = mesh_renderers.get<TransformComponent>(e);

      MeshUniform mesh_uniform;
      mesh_uniform.model = transform.getWorldTransform();
      mesh_uniform.light_count = light_count;

      cmd.mesh_idx = frame_meshes.size();
      frame_meshes.push_back(mesh_uniform);
    }

    {  // Write mesh asset
      cmd.mesh_asset = mesh_renderer.getMeshAsset();
    }

    frame.commands.push_back(cmd);
  }

  if (frame_materials.size() > 0) {
    for (uint32_t i = 0; i < frame_materials.size(); i++) {
      frame.material_buffer->writeElement(i, frame_materials[i]);
    }

    frame.material_descriptor = descriptor_pool->allocate(material_layout);
    frame.material_descriptor->updateDynamicBuffer(0, frame.material_buffer);
  }

  if (frame_meshes.size() > 0) {
    for (uint32_t i = 0; i < frame_meshes.size(); i++) {
      frame.mesh_buffer->writeElement(i, frame_meshes[i]);
    }

    frame.mesh_descriptor = descriptor_pool->allocate(mesh_layout);
    frame.mesh_descriptor->updateDynamicBuffer(0, frame.mesh_buffer);
    frame.mesh_descriptor->updateStorageBuffer(1, frame.point_lights);
  }
}

void MeshPass::render(uint32_t frame_index, VkCommandBuffer command_buffer,
                      const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = frame_data[frame_index];

  {
    GraphicsState graphics_state;

    graphics_state.input_assembly_state = {
        .primitive_topology = GraphicsState::PrimitiveTopology::TriangleList,
        .primitive_restart_enable = GraphicsState::BoolFlag::False};

    graphics_state.rasterization_state = {
        .polygon_mode = GraphicsState::PolygonMode::Fill,
        .cull_mode = GraphicsState::CullMode::Back};

    graphics_state.depth_state = {.test_enable = GraphicsState::BoolFlag::True,
                                  .write_enable = GraphicsState::BoolFlag::True,
                                  .compare_op = GraphicsState::CompareOp::Less};

    pipeline->cmdBind(command_buffer, graphics_state);
  }

  // TODO(marceline-cramer) GpuPipeline + GpuPipelineLayout
  viewport_descriptor->cmdBind(command_buffer, pipeline_layout, 0);

  for (auto& cmd : frame.commands) {
    log_zone_named("Render mesh");

    frame.material_descriptor->updateDynamicOffset(0, cmd.material_idx);
    frame.material_descriptor->cmdBind(command_buffer, pipeline_layout, 1);

    cmd.textures_descriptor->cmdBind(command_buffer, pipeline_layout, 2);

    frame.mesh_descriptor->updateDynamicOffset(0, cmd.mesh_idx);
    frame.mesh_descriptor->cmdBind(command_buffer, pipeline_layout, 3);

    const auto& mesh_asset = cmd.mesh_asset;

    VkBuffer vertex_buffers[] = {mesh_asset->vertex_buffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, mesh_asset->index_buffer->getBuffer(),
                         0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffer, mesh_asset->index_count, 1, 0, 0, 0);
  }
}

}  // namespace mondradiko
