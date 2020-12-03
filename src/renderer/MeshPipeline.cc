/**
 * @file MeshPipeline.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "renderer/MeshPipeline.h"

#include <vector>

#include "gpu/GpuInstance.h"
#include "gpu/GpuShader.h"
#include "log/log.h"
#include "renderer/Renderer.h"
#include "shaders/mesh.frag.h"
#include "shaders/mesh.vert.h"

namespace mondradiko {

MeshPipeline::MeshPipeline(GpuInstance* gpu,
                           GpuDescriptorSetLayout* viewport_layout,
                           VkRenderPass render_pass, uint32_t subpass_index)
    : gpu(gpu) {
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
    material_layout->addCombinedImageSampler(texture_sampler);
  }

  {
    log_zone_named("Create pipeline layout");

    std::vector<VkDescriptorSetLayout> set_layouts{
        viewport_layout->getSetLayout(), material_layout->getSetLayout()};

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
    log_zone_named("Create pipeline");

    GpuShader vert_shader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_mesh_vert,
                          sizeof(shaders_mesh_vert));
    GpuShader frag_shader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_mesh_frag,
                          sizeof(shaders_mesh_frag));

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        vert_shader.getStageCreateInfo(), frag_shader.getStageCreateInfo()};

    auto binding_description = MeshVertex::getBindingDescription();
    auto attribute_descriptions = MeshVertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = attribute_descriptions.size(),
        .pVertexAttributeDescriptions = attribute_descriptions.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    // TODO(marceline-cramer) Get viewport state from Viewport
    VkViewport viewport{.x = 0.0f,
                        .y = 0.0f,
                        .width = static_cast<float>(500),
                        .height = static_cast<float>(500),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f};

    VkRect2D scissor{.offset = {0, 0}, .extent = {500, 500}};

    VkPipelineViewportStateCreateInfo viewport_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor};

    VkPipelineRasterizationStateCreateInfo rasterization_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisample_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE};

    VkPipelineColorBlendAttachmentState color_blend_attachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

    VkPipelineColorBlendStateCreateInfo color_blend_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment};

    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data()};

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(shader_stages.size()),
        .pStages = shader_stages.data(),
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_info,
        .pRasterizationState = &rasterization_info,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_state_info,
        .layout = pipeline_layout,
        .renderPass = render_pass,
        .subpass = subpass_index,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(gpu->device, VK_NULL_HANDLE, 1,
                                  &pipeline_info, nullptr,
                                  &pipeline) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline.");
    }
  }

  {
    log_zone_named("Create buffers");

    material_buffer = new GpuBuffer(gpu, 128 * sizeof(MaterialUniform),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VMA_MEMORY_USAGE_CPU_TO_GPU);
  }
}

MeshPipeline::~MeshPipeline() {
  log_zone;

  if (material_buffer != nullptr) delete material_buffer;
  if (texture_sampler != VK_NULL_HANDLE)
    vkDestroySampler(gpu->device, texture_sampler, nullptr);
  if (pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(gpu->device, pipeline, nullptr);
  if (pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, pipeline_layout, nullptr);
  if (material_layout != nullptr) delete material_layout;
}

void MeshPipeline::allocateDescriptors(entt::registry& registry,
                                       GpuDescriptorPool* descriptor_pool) {
  log_zone;

  frame_materials.clear();
  auto mesh_renderers = registry.view<MeshRendererComponent>();
  for (auto e : mesh_renderers) {
    auto mesh_renderer = mesh_renderers.get(e);
    auto mesh_material = mesh_renderer.material_asset;
    auto iter = frame_materials.find(mesh_material);

    if (iter == frame_materials.end()) {
      GpuDescriptorSet* material_descriptor =
          descriptor_pool->allocate(material_layout);
      mesh_material->updateDescriptor(material_descriptor);
      frame_materials.emplace(mesh_material, material_descriptor);
    }
  }
}

void MeshPipeline::render(entt::registry& registry,
                          VkCommandBuffer commandBuffer,
                          GpuDescriptorSet* viewport_descriptor,
                          uint32_t viewport_offset) {
  log_zone;

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  // TODO(marceline-cramer) GpuPipeline + GpuPipelineLayout
  viewport_descriptor->updateDynamicOffset(0, viewport_offset);
  viewport_descriptor->cmdBind(commandBuffer, pipeline_layout, 0);

  // TODO(marceline-cramer) Actually update and use materials

  auto mesh_renderers = registry.view<MeshRendererComponent>();
  for (auto e : mesh_renderers) {
    log_zone_named("Render mesh");

    auto mesh_renderer = mesh_renderers.get(e);
    frame_materials.find(mesh_renderer.material_asset)
        ->second->cmdBind(commandBuffer, pipeline_layout, 1);

    VkBuffer vertex_buffers[] = {
        mesh_renderer.mesh_asset->vertex_buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         mesh_renderer.mesh_asset->index_buffer->buffer, 0,
                         VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, mesh_renderer.mesh_asset->index_count, 1,
                     0, 0, 0);
  }
}

}  // namespace mondradiko
