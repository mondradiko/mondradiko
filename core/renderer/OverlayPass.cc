// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/OverlayPass.h"

#include <vector>

#include "core/components/PointLightComponent.h"
#include "core/components/TransformComponent.h"
#include "core/cvars/BoolCVar.h"
#include "core/cvars/CVarScope.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GpuVector.h"
#include "core/ui/GlyphLoader.h"
#include "log/log.h"
#include "shaders/debug.frag.h"
#include "shaders/debug.vert.h"
#include "shaders/glyph.frag.h"
#include "shaders/glyph.vert.h"

namespace mondradiko {

void OverlayPass::initCVars(CVarScope* cvars) {
  CVarScope* debug = cvars->addChild("debug");

  debug->addValue<BoolCVar>("enabled");
  debug->addValue<BoolCVar>("draw_lights");
  debug->addValue<BoolCVar>("draw_transforms");
}

OverlayPass::OverlayPass(const CVarScope* cvars, const GlyphLoader* glyphs,
                         GpuInstance* gpu,
                         GpuDescriptorSetLayout* viewport_layout,
                         VkRenderPass parent_pass, uint32_t subpass_index)
    : cvars(cvars->getChild("debug")), glyphs(glyphs), gpu(gpu) {
  log_zone;

  {
    log_zone_named("Create debug pipeline layout");

    std::vector<VkDescriptorSetLayout> set_layouts{
        viewport_layout->getSetLayout(),
    };

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(set_layouts.size()),
        .pSetLayouts = set_layouts.data()};

    if (vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr,
                               &debug_pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout.");
    }
  }

  {
    log_zone_named("Create glyph pipeline layout");

    glyph_set_layout = new GpuDescriptorSetLayout(gpu);
    glyph_set_layout->addCombinedImageSampler(glyphs->getSampler());
    glyph_set_layout->addStorageBuffer(sizeof(GlyphUniform));

    std::vector<VkDescriptorSetLayout> set_layouts{
        viewport_layout->getSetLayout(), glyph_set_layout->getSetLayout()};

    VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(set_layouts.size()),
        .pSetLayouts = set_layouts.data()};

    if (vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr,
                               &glyph_pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout.");
    }
  }

  {
    log_zone_named("Create debug pipeline");

    GpuShader vert_shader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_debug_vert,
                          sizeof(shaders_debug_vert));
    GpuShader frag_shader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_debug_frag,
                          sizeof(shaders_debug_frag));

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        vert_shader.getStageCreateInfo(), frag_shader.getStageCreateInfo()};

    auto binding_description = DebugDrawVertex::getBindingDescription();
    auto attribute_descriptions = DebugDrawVertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = attribute_descriptions.size(),
        .pVertexAttributeDescriptions = attribute_descriptions.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
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
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE};

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
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_state_info,
        .layout = debug_pipeline_layout,
        .renderPass = parent_pass,
        .subpass = subpass_index,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(gpu->device, VK_NULL_HANDLE, 1,
                                  &pipeline_info, nullptr,
                                  &debug_pipeline) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline.");
    }
  }

  {
    log_zone_named("Create glyph pipeline");

    GpuShader vert_shader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_glyph_vert,
                          sizeof(shaders_glyph_vert));
    GpuShader frag_shader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_glyph_frag,
                          sizeof(shaders_glyph_frag));

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        vert_shader.getStageCreateInfo(), frag_shader.getStageCreateInfo()};

    auto binding_description = GlyphInstance::getBindingDescription();
    auto attribute_descriptions = GlyphInstance::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = attribute_descriptions.size(),
        .pVertexAttributeDescriptions = attribute_descriptions.data()};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE};

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
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_state_info,
        .layout = glyph_pipeline_layout,
        .renderPass = parent_pass,
        .subpass = subpass_index,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1};

    if (vkCreateGraphicsPipelines(gpu->device, VK_NULL_HANDLE, 1,
                                  &pipeline_info, nullptr,
                                  &glyph_pipeline) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline.");
    }
  }
}

OverlayPass::~OverlayPass() {
  log_zone;

  if (debug_pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(gpu->device, debug_pipeline, nullptr);
  if (debug_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, debug_pipeline_layout, nullptr);
  if (glyph_pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(gpu->device, glyph_pipeline, nullptr);
  if (glyph_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, glyph_pipeline_layout, nullptr);
  if (glyph_set_layout != nullptr) delete glyph_set_layout;
}

void OverlayPass::createFrameData(OverlayPassFrameData& frame) {
  log_zone;

  frame.debug_vertices = new GpuVector(gpu, sizeof(DebugDrawVertex),
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  frame.debug_indices = new GpuVector(gpu, sizeof(DebugDrawIndex),
                                      VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  frame.glyph_instances = new GpuVector(gpu, sizeof(GlyphInstance),
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void OverlayPass::destroyFrameData(OverlayPassFrameData& frame) {
  log_zone;

  if (frame.debug_vertices != nullptr) delete frame.debug_vertices;
  if (frame.debug_indices != nullptr) delete frame.debug_indices;
  if (frame.glyph_instances != nullptr) delete frame.glyph_instances;
}

void OverlayPass::allocateDescriptors(EntityRegistry& registry,
                                      OverlayPassFrameData& frame,
                                      AssetPool* asset_pool,
                                      GpuDescriptorPool* descriptor_pool) {
  log_zone;

  frame.index_count = 0;
  DebugDrawIndex vertex_count = 0;

  frame.glyph_descriptor = descriptor_pool->allocate(glyph_set_layout);
  frame.glyph_descriptor->updateImage(0, glyphs->getAtlas());
  frame.glyph_descriptor->updateStorageBuffer(1, glyphs->getGlyphs());

  frame.glyph_count = 0;

  GlyphString test_string;
  glyphs->drawString(&test_string,
                     "The quick brown fox jumps over the lazy dog.");

  for (uint32_t i = 0; i < test_string.size(); i++) {
    frame.glyph_instances->writeElement(frame.glyph_count, test_string[i]);
    frame.glyph_count++;
  }

  if (!cvars->get<BoolCVar>("enabled")) return;

  if (cvars->get<BoolCVar>("draw_transforms")) {
    auto transform_view = registry.view<TransformComponent>();

    for (auto& e : transform_view) {
      glm::mat4 transform = transform_view.get(e).getWorldTransform();

      {
        // Draw X line

        DebugDrawVertex vertex1{
            .position = transform * glm::vec4(1.0, 0.0, 0.0, 1.0),
            .color = glm::vec3(1.0, 0.0, 0.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{
            .position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0),
            .color = glm::vec3(1.0, 0.0, 0.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        // Draw Y line

        DebugDrawVertex vertex1{
            .position = transform * glm::vec4(0.0, 1.0, 0.0, 1.0),
            .color = glm::vec3(0.0, 1.0, 0.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{
            .position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0),
            .color = glm::vec3(0.0, 1.0, 0.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        // Draw Z line

        DebugDrawVertex vertex1{
            .position = transform * glm::vec4(0.0, 0.0, 1.0, 1.0),
            .color = glm::vec3(0.0, 0.0, 1.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{
            .position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0),
            .color = glm::vec3(0.0, 0.0, 1.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }
    }
  }

  if (cvars->get<BoolCVar>("draw_lights")) {
    auto point_lights_view = registry.view<PointLightComponent>();

    for (auto e : point_lights_view) {
      auto& point_light = point_lights_view.get(e);

      PointLightUniform uniform;
      point_light.getUniform(&uniform);
      glm::vec3 position =
          glm::vec3(uniform.position.x, uniform.position.y, uniform.position.z);

      {
        DebugDrawVertex vertex{.position = position + glm::vec3(0.0, 0.1, 0.0),
                               .color = glm::vec3(1.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        DebugDrawVertex vertex{.position = position, .color = glm::vec3(1.0)};

        frame.debug_vertices->writeElement(vertex_count, vertex);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }
    }
  }
}

void OverlayPass::render(EntityRegistry& registry, OverlayPassFrameData& frame,
                         AssetPool* asset_pool, VkCommandBuffer command_buffer,
                         GpuDescriptorSet* viewport_descriptor,
                         uint32_t viewport_offset) {
  log_zone;

  {
    log_zone_named("Render debug");

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      debug_pipeline);

    // TODO(marceline-cramer) GpuPipeline + GpuPipelineLayout
    viewport_descriptor->updateDynamicOffset(0, viewport_offset);
    viewport_descriptor->cmdBind(command_buffer, debug_pipeline_layout, 0);

    VkBuffer vertex_buffers[] = {frame.debug_vertices->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, frame.debug_indices->getBuffer(), 0,
                         VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, frame.index_count, 1, 0, 0, 0);
  }

  {
    log_zone_named("Render glyphs");

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      glyph_pipeline);

    viewport_descriptor->cmdBind(command_buffer, glyph_pipeline_layout, 0);
    frame.glyph_descriptor->cmdBind(command_buffer, glyph_pipeline_layout, 1);

    VkBuffer vertex_buffers[] = {frame.glyph_instances->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdDraw(command_buffer, 4, frame.glyph_count, 0, 0);
  }
}

}  // namespace mondradiko
