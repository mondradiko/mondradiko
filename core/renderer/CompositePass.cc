// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/CompositePass.h"

#include "core/displays/Viewport.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuPipeline.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GraphicsState.h"
#include "core/renderer/Renderer.h"
#include "core/shaders/composite.frag.h"
#include "core/shaders/composite.vert.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

CompositePass::CompositePass(Renderer* renderer)
    : gpu(renderer->getGpu()), renderer(renderer) {
  log_zone;

  {
    log_zone_named("Create set layout");

    _set_layout = new GpuDescriptorSetLayout(gpu);
    _set_layout->addInputAttachment();
    _set_layout->addInputAttachment();
  }

  {
    log_zone_named("Create pipeline layout");

    types::vector<VkDescriptorSetLayout> set_layouts{
        renderer->getViewportLayout()->getSetLayout(),
        _set_layout->getSetLayout()};

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    layoutInfo.pSetLayouts = set_layouts.data();

    if (vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr,
                               &_pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout.");
    }
  }

  {
    log_zone_named("Create shaders");

    _vertex_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_composite_vert,
                      sizeof(shaders_composite_vert));
    _fragment_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_composite_frag,
                      sizeof(shaders_composite_frag));
  }

  {
    log_zone_named("Create pipelines");

    GpuPipeline::VertexBindings vertex_bindings{};
    GpuPipeline::AttributeDescriptions attribute_descriptions{};

    _pipeline = new GpuPipeline(
        gpu, _pipeline_layout, renderer->getCompositeRenderPass(),
        renderer->getCompositeSubpass(), _vertex_shader, _fragment_shader,
        vertex_bindings, attribute_descriptions);
  }
}

CompositePass::~CompositePass() {
  log_zone;

  if (_pipeline != nullptr) delete _pipeline;
  if (_vertex_shader != nullptr) delete _vertex_shader;
  if (_fragment_shader != nullptr) delete _fragment_shader;
  if (_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, _pipeline_layout, nullptr);
  if (_set_layout != nullptr) delete _set_layout;
}

void CompositePass::createFrameData(uint32_t frame_count) {
  log_zone;

  _frame_data.resize(frame_count);
}

void CompositePass::beginFrame(uint32_t frame_index, uint32_t viewport_count,
                               GpuDescriptorPool* descriptor_pool) {
  log_zone;

  renderer->addPassToPhase(RenderPhase::Composite, this);

  _current_frame = frame_index;
  auto& frame = _frame_data[_current_frame];

  frame.input_attachments.resize(viewport_count);

  for (uint32_t i = 0; i < viewport_count; i++) {
    auto& input_attachment = frame.input_attachments[i];
    input_attachment = descriptor_pool->allocate(_set_layout);
  }
}

void CompositePass::renderViewport(
    VkCommandBuffer command_buffer, uint32_t viewport_index, RenderPhase,
    const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = _frame_data[_current_frame];

  {
    auto gs = GraphicsState::CreateGenericOpaque();
    gs.input_assembly_state.primitive_topology =
        GraphicsState::PrimitiveTopology::TriangleStrip;
    gs.depth_state.test_enable = GraphicsState::BoolFlag::False;
    _pipeline->cmdBind(command_buffer, gs);
  }

  auto& input_attachment = frame.input_attachments[viewport_index];

  Viewport* viewport = renderer->getCurrentViewport(viewport_index);
  input_attachment->updateInputAttachment(0, viewport->getHdrImage());
  input_attachment->updateInputAttachment(1, viewport->getOverlayImage());

  viewport_descriptor->cmdBind(command_buffer, _pipeline_layout, 0);
  input_attachment->cmdBind(command_buffer, _pipeline_layout, 1);

  vkCmdDraw(command_buffer, 4, 1, 0, 0);
}

}  // namespace core
}  // namespace mondradiko
