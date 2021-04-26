// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/OverlayPass.h"

#include "core/components/internal/WorldTransform.h"
#include "core/components/scriptable/PointLightComponent.h"
#include "core/cvars/BoolCVar.h"
#include "core/cvars/CVarScope.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GpuVector.h"
#include "core/gpu/GraphicsState.h"
#include "core/renderer/DebugDraw.h"
#include "core/renderer/Renderer.h"
#include "core/ui/GlyphLoader.h"
#include "core/world/World.h"
#include "log/log.h"
#include "shaders/debug.frag.h"
#include "shaders/debug.vert.h"

namespace mondradiko {
namespace core {

void OverlayPass::initCVars(CVarScope* cvars) {
  CVarScope* debug = cvars->addChild("debug");

  debug->addValue<BoolCVar>("enabled");
  debug->addValue<BoolCVar>("draw_lights");
  debug->addValue<BoolCVar>("draw_transforms");
}

OverlayPass::OverlayPass(const CVarScope* cvars, const GlyphLoader* glyphs,
                         Renderer* renderer, World* world)
    : cvars(cvars->getChild("debug")),
      glyphs(glyphs),
      gpu(renderer->getGpu()),
      renderer(renderer),
      world(world) {
  log_zone;

  {
    log_zone_named("Create debug pipeline layout");

    types::vector<VkDescriptorSetLayout> set_layouts{
        renderer->getViewportLayout()->getSetLayout(),
    };

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    layout_info.pSetLayouts = set_layouts.data();

    if (vkCreatePipelineLayout(gpu->device, &layout_info, nullptr,
                               &debug_pipeline_layout) != VK_SUCCESS) {
      log_ftl("Failed to create pipeline layout.");
    }
  }

  {
    log_zone_named("Create debug shaders");

    debug_vertex_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_debug_vert,
                      sizeof(shaders_debug_vert));
    debug_fragment_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_debug_frag,
                      sizeof(shaders_debug_frag));
  }

  {
    log_zone_named("Create debug pipeline");

    auto vertex_bindings = DebugDrawList::Vertex::getVertexBindings();
    auto attribute_descriptions =
        DebugDrawList::Vertex::getAttributeDescriptions();

    debug_pipeline = new GpuPipeline(
        gpu, debug_pipeline_layout, renderer->getViewportRenderPass(),
        renderer->getForwardSubpass(), debug_vertex_shader,
        debug_fragment_shader, vertex_bindings, attribute_descriptions);
  }
}

OverlayPass::~OverlayPass() {
  log_zone;

  if (debug_vertex_shader != nullptr) delete debug_vertex_shader;
  if (debug_fragment_shader != nullptr) delete debug_fragment_shader;
  if (debug_pipeline != nullptr) delete debug_pipeline;
  if (debug_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, debug_pipeline_layout, nullptr);
}

void OverlayPass::createFrameData(uint32_t frame_count) {
  log_zone;

  frame_data.resize(frame_count);

  for (auto& frame : frame_data) {
    frame.debug_vertices = new GpuVector(gpu, sizeof(DebugDrawList::Vertex),
                                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    frame.debug_indices = new GpuVector(gpu, sizeof(DebugDrawList::Index),
                                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  }
}

void OverlayPass::destroyFrameData() {
  log_zone;

  for (auto& frame : frame_data) {
    if (frame.debug_vertices != nullptr) delete frame.debug_vertices;
    if (frame.debug_indices != nullptr) delete frame.debug_indices;
  }
}

void OverlayPass::beginFrame(uint32_t frame_index,
                             GpuDescriptorPool* descriptor_pool) {
  log_zone;

  renderer->addPassToPhase(RenderPhase::Forward, this);

  current_frame = frame_index;
  auto& frame = frame_data[current_frame];

  frame.index_count = 0;

  if (!cvars->get<BoolCVar>("enabled")) return;

  DebugDrawList debug_draw;

  if (cvars->get<BoolCVar>("draw_transforms")) {
    auto transform_view = world->registry.view<WorldTransform>();

    for (auto& e : transform_view) {
      glm::mat4 transform = transform_view.get(e).getTransform();

      glm::vec3 origin = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
      glm::vec3 x_axis = transform * glm::vec4(1.0, 0.0, 0.0, 1.0);
      glm::vec3 y_axis = transform * glm::vec4(0.0, 1.0, 0.0, 1.0);
      glm::vec3 z_axis = transform * glm::vec4(0.0, 0.0, 1.0, 1.0);

      debug_draw.drawLine(origin, x_axis, glm::vec3(1.0, 0.0, 0.0));
      debug_draw.drawLine(origin, y_axis, glm::vec3(0.0, 1.0, 0.0));
      debug_draw.drawLine(origin, z_axis, glm::vec3(0.0, 0.0, 1.0));
    }
  }

  if (cvars->get<BoolCVar>("draw_lights")) {
    auto point_lights_view = world->registry.view<PointLightComponent>();

    for (auto e : point_lights_view) {
      auto& point_light = point_lights_view.get(e);

      glm::mat4 world_transform(1.0);
      if (world->registry.has<WorldTransform>(e)) {
        world_transform = world->registry.get<WorldTransform>(e).getTransform();
      }

      PointLightUniform uniform;
      point_light.getUniform(&uniform);
      glm::vec3 position =
          world_transform * glm::vec4(glm::vec3(uniform.position), 1.0);

      glm::vec3 line_space(0.0, 0.1, 0.0);
      glm::vec3 color(1.0, 1.0, 1.0);

      debug_draw.drawLine(position - line_space, position + line_space, color);
    }
  }

  frame.index_count =
      debug_draw.writeData(frame.debug_vertices, frame.debug_indices);
}

void OverlayPass::renderViewport(RenderPhase phase,
                                 VkCommandBuffer command_buffer,
                                 const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = frame_data[current_frame];

  {
    log_zone_named("Render debug");

    {
      GraphicsState graphics_state;

      GraphicsState::InputAssemblyState input_assembly_state{};
      input_assembly_state.primitive_topology =
          GraphicsState::PrimitiveTopology::LineList;
      input_assembly_state.primitive_restart_enable =
          GraphicsState::BoolFlag::False;
      graphics_state.input_assembly_state = input_assembly_state;

      GraphicsState::RasterizatonState rasterization_state{};
      rasterization_state.polygon_mode = GraphicsState::PolygonMode::Fill;
      rasterization_state.cull_mode = GraphicsState::CullMode::None;
      graphics_state.rasterization_state = rasterization_state;

      GraphicsState::DepthState depth_state{};
      depth_state.test_enable = GraphicsState::BoolFlag::False;
      depth_state.write_enable = GraphicsState::BoolFlag::False;
      depth_state.compare_op = GraphicsState::CompareOp::Always;
      graphics_state.depth_state = depth_state;

      debug_pipeline->cmdBind(command_buffer, graphics_state);
    }

    // TODO(marceline-cramer) GpuPipeline + GpuPipelineLayout
    viewport_descriptor->cmdBind(command_buffer, debug_pipeline_layout, 0);

    VkBuffer vertex_buffers[] = {frame.debug_vertices->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, frame.debug_indices->getBuffer(), 0,
                         VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, frame.index_count, 1, 0, 0, 0);
  }
}

}  // namespace core
}  // namespace mondradiko
