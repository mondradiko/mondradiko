// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/OverlayPass.h"

#include "core/components/internal/PointerComponent.h"
#include "core/components/internal/WorldTransform.h"
#include "core/components/scriptable/PointLightComponent.h"
#include "core/components/synchronized/ShapeComponent.h"
#include "core/cvars/BoolCVar.h"
#include "core/cvars/CVarScope.h"
#include "core/displays/Viewport.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuShader.h"
#include "core/gpu/GpuVector.h"
#include "core/gpu/GraphicsState.h"
#include "core/renderer/DebugDrawList.h"
#include "core/renderer/Renderer.h"
#include "core/world/World.h"
#include "log/log.h"
#include "shaders/debug.frag.h"
#include "shaders/debug.vert.h"

namespace mondradiko {
namespace core {

void OverlayPass::initCVars(CVarScope* cvars) {
  CVarScope* debug = cvars->addChild("debug");

  debug->addValue<BoolCVar>("enabled");
  debug->addValue<BoolCVar>("draw_grid");
  debug->addValue<BoolCVar>("draw_lights");
  debug->addValue<BoolCVar>("draw_lights_aoe");
  debug->addValue<BoolCVar>("draw_shapes");
  debug->addValue<BoolCVar>("draw_transforms");
  debug->addValue<BoolCVar>("draw_pointers");
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
        gpu, debug_pipeline_layout, renderer->getMainRenderPass(),
        renderer->getOverlaySubpass(), debug_vertex_shader,
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

void OverlayPass::beginFrame(uint32_t frame_index, uint32_t viewport_count,
                             GpuDescriptorPool* descriptor_pool) {
  log_zone;

  renderer->addPassToPhase(RenderPhase::Overlay, this);

  current_frame = frame_index;
  auto& frame = frame_data[current_frame];

  frame.index_count = 0;

  if (!cvars->get<BoolCVar>("enabled")) return;

  if (cvars->get<BoolCVar>("draw_grid")) {
    int grid_width = 10;
    int grid_height = 10;
    float grid_space = 1.0;
    glm::vec4 color = glm::vec4(0.0, 1.0, 0.0, 1.0);

    // X lines
    for (int x = -grid_width; x <= grid_width; x++) {
      float y = grid_height * grid_space;
      glm::vec3 start = glm::vec3(x * grid_space, 0.0, y);
      glm::vec3 end = glm::vec3(x * grid_space, 0.0, -y);
      _debug_draw.drawLine(start, end, color);
    }

    // Y lines
    for (int y = -grid_height; y <= grid_height; y++) {
      float x = grid_width * grid_space;
      glm::vec3 start = glm::vec3(x, 0.0, y * grid_space);
      glm::vec3 end = glm::vec3(-x, 0.0, y * grid_space);
      _debug_draw.drawLine(start, end, color);
    }
  }

  if (cvars->get<BoolCVar>("draw_transforms")) {
    log_zone_named("Draw transforms");

    auto transform_view = world->registry.view<WorldTransform>();

    for (auto& e : transform_view) {
      glm::mat4 transform = transform_view.get(e).getTransform();

      glm::vec3 origin = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
      glm::vec3 x_axis = transform * glm::vec4(1.0, 0.0, 0.0, 1.0);
      glm::vec3 y_axis = transform * glm::vec4(0.0, 1.0, 0.0, 1.0);
      glm::vec3 z_axis = transform * glm::vec4(0.0, 0.0, 1.0, 1.0);

      _debug_draw.drawLine(origin, x_axis, glm::vec3(1.0, 0.0, 0.0));
      _debug_draw.drawLine(origin, y_axis, glm::vec3(0.0, 1.0, 0.0));
      _debug_draw.drawLine(origin, z_axis, glm::vec3(0.0, 0.0, 1.0));
    }
  }

  if (cvars->get<BoolCVar>("draw_shapes")) {
    log_zone_named("Draw shapes");

    auto shapes_view = world->registry.view<ShapeComponent>();

    for (auto e : shapes_view) {
      auto& shape = shapes_view.get(e);

      if (!shape.isLoaded()) continue;

      glm::vec3 color = glm::vec3(0.8, 0.8, 1.0);

      glm::mat4 world_transform(1.0);
      if (world->registry.has<WorldTransform>(e)) {
        world_transform = world->registry.get<WorldTransform>(e).getTransform();
      }

      const AnyShape& any_shape = shape.getShape()->getAnyShape();

      AnyShape::debugDraw(&any_shape, world_transform, color, &_debug_draw);
    }
  }

  if (cvars->get<BoolCVar>("draw_lights")) {
    log_zone_named("Draw point lights");

    auto point_lights_view = world->registry.view<PointLightComponent>();

    for (auto e : point_lights_view) {
      auto& point_light = point_lights_view.get(e);

      glm::vec3 color = point_light.getSaturatedColor();

      glm::mat4 world_transform(1.0);
      if (world->registry.has<WorldTransform>(e)) {
        world_transform = world->registry.get<WorldTransform>(e).getTransform();
      }

      PointLightUniform uniform;
      point_light.getUniform(&uniform);
      glm::vec3 position =
          world_transform * glm::vec4(glm::vec3(uniform.position), 1.0);

      _debug_draw.drawIcosahedron(position, 0.1, color);
    }
  }

  if (cvars->get<BoolCVar>("draw_lights_aoe")) {
    log_zone_named("Draw point light areas of effect");

    auto point_lights_view = world->registry.view<PointLightComponent>();

    for (auto e : point_lights_view) {
      auto& point_light = point_lights_view.get(e);

      glm::vec3 color = point_light.getSaturatedColor();

      glm::mat4 world_transform(1.0);
      if (world->registry.has<WorldTransform>(e)) {
        world_transform = world->registry.get<WorldTransform>(e).getTransform();
      }

      const auto& aoe = point_light.getAreaOfEffect();
      SphereShape::debugDraw(&aoe, world_transform, color, &_debug_draw);
    }
  }

  if (cvars->get<BoolCVar>("draw_pointers")) {
    auto pointers_view = world->registry.view<PointerComponent>();

    for (auto e : pointers_view) {
      auto& pointer = pointers_view.get(e);

      glm::mat4 world_transform(1.0);
      if (world->registry.has<WorldTransform>(e)) {
        world_transform = world->registry.get<WorldTransform>(e).getTransform();
      }

      glm::vec3 position = pointer.getPosition();
      glm::vec3 direction = position + pointer.getDirection() * glm::vec3(10.0);

      glm::vec3 start_pos = world_transform * glm::vec4(position, 1.0);
      glm::vec3 start_color = glm::vec3(0.0, 1.0, 1.0);

      glm::vec3 end_pos = world_transform * glm::vec4(direction, 1.0);
      glm::vec3 end_color = glm::vec3(0.5, 0.5, 0.5);

      auto start_vertex = _debug_draw.makeVertex(start_pos, start_color);
      auto end_vertex = _debug_draw.makeVertex(end_pos, end_color);

      _debug_draw.drawLine(start_vertex, end_vertex);
    }
  }

  frame.index_count =
      _debug_draw.writeData(frame.debug_vertices, frame.debug_indices);
  _debug_draw.clear();
}

void OverlayPass::renderViewport(VkCommandBuffer command_buffer,
                                 uint32_t viewport_index, RenderPhase phase,
                                 const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = frame_data[current_frame];

  {
    log_zone_named("Render debug");

    {
      auto gs = GraphicsState::CreateGenericOpaque();
      gs.input_assembly_state.primitive_topology =
          GraphicsState::PrimitiveTopology::LineList;
      gs.multisample_state.rasterization_samples =
          renderer->getCurrentViewport(viewport_index)->getSampleCount();
      gs.depth_state.write_enable = GraphicsState::BoolFlag::False;
      debug_pipeline->cmdBind(command_buffer, gs);
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
