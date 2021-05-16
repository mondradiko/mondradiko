// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuPipeline.h"

#include <xxhash.h>  // NOLINT

#include <array>
#include <cstring>

#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GraphicsState.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

GpuPipeline::GpuPipeline(GpuInstance* gpu, VkPipelineLayout pipeline_layout,
                         VkRenderPass render_pass, uint32_t subpass_index,
                         const GpuShader* vertex_shader,
                         const GpuShader* fragment_shader,
                         const VertexBindings& vertex_bindings,
                         const AttributeDescriptions& attribute_descriptions)
    : gpu(gpu),
      pipeline_layout(pipeline_layout),
      render_pass(render_pass),
      subpass_index(subpass_index),
      vertex_shader(vertex_shader),
      fragment_shader(fragment_shader),
      vertex_bindings(vertex_bindings),
      attribute_descriptions(attribute_descriptions) {
  log_zone;
}

GpuPipeline::~GpuPipeline() {
  log_zone;

  for (auto pipeline : pipelines) {
    vkDestroyPipeline(gpu->device, pipeline.second, nullptr);
  }
}

GpuPipeline::StateHash GpuPipeline::createPipeline(
    const GraphicsState& graphics_state) {
  log_zone;

  auto state_hash = getStateHash(graphics_state);

  auto iter = pipelines.find(state_hash);
  if (iter != pipelines.end()) return state_hash;

  log_inf_fmt("Creating pipeline: 0x%0lx", state_hash);

  VkPipeline pipeline_object;

  types::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
      vertex_shader->getStageCreateInfo(),
      fragment_shader->getStageCreateInfo()};

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertex_bindings.size());
  vertex_input_info.pVertexBindingDescriptions = vertex_bindings.data();
  vertex_input_info.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexAttributeDescriptions =
      attribute_descriptions.data();

  auto input_assembly_info = graphics_state.createVkInputAssemblyState();

  // TODO(marceline-cramer) Get viewport state from Viewport
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(500);
  viewport.height = static_cast<float>(500);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {500, 500};

  VkPipelineViewportStateCreateInfo viewport_info{};
  viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.viewportCount = 1;
  viewport_info.pViewports = &viewport;
  viewport_info.scissorCount = 1;
  viewport_info.pScissors = &scissor;

  auto rasterization_info = graphics_state.createVkRasterizationState();
  auto multisample_info = graphics_state.createVkMultisampleState();
  auto depth_stencil_info = graphics_state.createVkDepthStencilState();
  auto color_blend_attachment = graphics_state.createVkColorBlendState();

  VkPipelineColorBlendStateCreateInfo color_blend_info{};
  color_blend_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.attachmentCount = 1;
  color_blend_info.pAttachments = &color_blend_attachment;

  types::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_info{};
  dynamic_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount =
      static_cast<uint32_t>(dynamic_states.size());
  dynamic_state_info.pDynamicStates = dynamic_states.data();

  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
  pipeline_info.pStages = shader_stages.data();
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly_info;
  pipeline_info.pViewportState = &viewport_info;
  pipeline_info.pRasterizationState = &rasterization_info;
  pipeline_info.pMultisampleState = &multisample_info;
  pipeline_info.pDepthStencilState = &depth_stencil_info;
  pipeline_info.pColorBlendState = &color_blend_info;
  pipeline_info.pDynamicState = &dynamic_state_info;
  pipeline_info.layout = pipeline_layout;
  pipeline_info.renderPass = render_pass;
  pipeline_info.subpass = subpass_index;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(gpu->device, VK_NULL_HANDLE, 1, &pipeline_info,
                                nullptr, &pipeline_object) != VK_SUCCESS) {
    log_ftl_fmt("Failed to create pipeline 0x%0lx", state_hash);
  }

  pipelines.emplace(state_hash, pipeline_object);
  return state_hash;
}

GpuPipeline::StateHash GpuPipeline::getStateHash(
    const GraphicsState& graphics_state) {
  log_zone;
  return XXH64(&graphics_state, sizeof(GraphicsState), 0);
}

void GpuPipeline::cmdBind(VkCommandBuffer command_buffer,
                          StateHash state_hash) {
  log_zone;

  auto iter = pipelines.find(state_hash);
  if (iter == pipelines.end()) {
    log_ftl_fmt("Pipeline 0x%0lx not found", state_hash);
  }

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    iter->second);
}

void GpuPipeline::cmdBind(VkCommandBuffer command_buffer,
                          const GraphicsState& graphics_state) {
  log_zone;

  StateHash state_hash = getStateHash(graphics_state);

  auto iter = pipelines.find(state_hash);
  if (iter == pipelines.end()) {
    log_wrn_fmt("Pipeline cache miss: 0x%0lx", state_hash);
    createPipeline(graphics_state);
  }

  cmdBind(command_buffer, state_hash);
}

}  // namespace core
}  // namespace mondradiko
