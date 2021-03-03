// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UserInterface.h"

#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuPipeline.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GraphicsState.h"
#include "core/renderer/Renderer.h"
#include "core/scripting/ScriptEnvironment.h"
#include "core/shaders/panel.frag.h"
#include "core/shaders/panel.vert.h"
#include "core/ui/GlyphLoader.h"
#include "core/ui/UiPanel.h"
#include "log/log.h"

namespace mondradiko {

UserInterface::UserInterface(GlyphLoader* glyphs, Renderer* renderer)
    : glyphs(glyphs), gpu(renderer->getGpu()), renderer(renderer) {
  log_zone;

  {
    log_zone_named("Bind script API");

    scripts = new ScriptEnvironment;
    scripts->linkUiApis(this);
  }

  // Temp panel
  main_panel = new UiPanel(glyphs, scripts);

  {
    log_zone_named("Create shaders");

    panel_vertex_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_panel_vert,
                      sizeof(shaders_panel_vert));
    panel_fragment_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_panel_frag,
                      sizeof(shaders_panel_frag));
  }

  {
    log_zone_named("Create panel pipeline layout");

    std::vector<VkDescriptorSetLayout> set_layouts{
        renderer->getViewportLayout()->getSetLayout()};

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    layout_info.pSetLayouts = set_layouts.data();

    if (vkCreatePipelineLayout(gpu->device, &layout_info, nullptr,
                               &panel_pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout");
    }
  }

  {
    log_zone_named("Create pipelines");

    GpuPipeline::VertexBindings vertex_bindings;
    GpuPipeline::AttributeDescriptions attribute_descriptions;

    panel_pipeline = new GpuPipeline(
        gpu, panel_pipeline_layout, renderer->getViewportRenderPass(),
        renderer->getForwardSubpass(), panel_vertex_shader,
        panel_fragment_shader, vertex_bindings, attribute_descriptions);
  }
}

UserInterface::~UserInterface() {
  log_zone;

  if (panel_pipeline != nullptr) delete panel_pipeline;
  if (panel_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, panel_pipeline_layout, nullptr);
  if (panel_vertex_shader != nullptr) delete panel_vertex_shader;
  if (panel_fragment_shader != nullptr) delete panel_fragment_shader;
  if (main_panel != nullptr) delete main_panel;
  if (scripts != nullptr) delete scripts;
}

void UserInterface::createFrameData(uint32_t frame_count) {
  log_zone;

  frame_data.resize(frame_count);

  for (auto& frame : frame_data) {
    frame.panel_atlas = new GpuImage(
        gpu, VK_FORMAT_R8G8B8A8_SRGB, 256, 256, 1,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);
  }
}

void UserInterface::destroyFrameData() {
  log_zone;

  for (auto& frame : frame_data) {
    if (frame.panel_atlas != nullptr) delete frame.panel_atlas;
  }
}

void UserInterface::beginFrame(uint32_t frame_index,
                               GpuDescriptorPool* descriptor_pool) {
  log_zone;

  renderer->addPassToPhase(RenderPhase::Forward, this);

  current_frame = frame_index;
  auto& frame = frame_data[current_frame];
}

void UserInterface::renderViewport(
    RenderPhase phase, VkCommandBuffer command_buffer,
    const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = frame_data[current_frame];

  {
    log_zone_named("Render panels");

    {
      GraphicsState graphics_state;

      GraphicsState::InputAssemblyState input_assembly_state{};
      input_assembly_state.primitive_topology =
          GraphicsState::PrimitiveTopology::TriangleStrip;
      input_assembly_state.primitive_restart_enable =
          GraphicsState::BoolFlag::False;
      graphics_state.input_assembly_state = input_assembly_state;

      GraphicsState::RasterizatonState rasterization_state{};
      rasterization_state.polygon_mode = GraphicsState::PolygonMode::Fill;
      rasterization_state.cull_mode = GraphicsState::CullMode::None;
      graphics_state.rasterization_state = rasterization_state;

      GraphicsState::DepthState depth_state{};
      depth_state.test_enable = GraphicsState::BoolFlag::True;
      depth_state.write_enable = GraphicsState::BoolFlag::True;
      depth_state.compare_op = GraphicsState::CompareOp::Less;
      graphics_state.depth_state = depth_state;

      panel_pipeline->cmdBind(command_buffer, graphics_state);
    }

    viewport_descriptor->cmdBind(command_buffer, panel_pipeline_layout, 0);
    vkCmdDraw(command_buffer, 4, 1, 0, 0);
  }
}

}  // namespace mondradiko
