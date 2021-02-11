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
#include "core/gpu/GraphicsState.h"
#include "core/renderer/Renderer.h"
#include "core/ui/GlyphLoader.h"
#include "core/world/World.h"
#include "log/log.h"
#include "shaders/debug.frag.h"
#include "shaders/debug.vert.h"

namespace mondradiko {

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

    std::vector<VkDescriptorSetLayout> set_layouts{
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
    log_zone_named("Create glyph pipeline layout");

    glyph_set_layout = new GpuDescriptorSetLayout(gpu);
    glyph_set_layout->addCombinedImageSampler(glyphs->getSampler());
    glyph_set_layout->addStorageBuffer(sizeof(GlyphUniform));

    std::vector<VkDescriptorSetLayout> set_layouts{
        renderer->getViewportLayout()->getSetLayout(),
        glyph_set_layout->getSetLayout()};

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    layout_info.pSetLayouts = set_layouts.data();

    if (vkCreatePipelineLayout(gpu->device, &layout_info, nullptr,
                               &glyph_pipeline_layout) != VK_SUCCESS) {
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

    auto vertex_bindings = DebugDrawVertex::getVertexBindings();
    auto attribute_descriptions = DebugDrawVertex::getAttributeDescriptions();

    debug_pipeline = new GpuPipeline(gpu, debug_pipeline_layout,
                                     renderer->getCompositePass(), 0,
                                     debug_vertex_shader, debug_fragment_shader,
                                     vertex_bindings, attribute_descriptions);
  }

  {
    log_zone_named("Create glyph pipeline");

    auto vertex_bindings = GlyphInstance::getVertexBindings();
    auto attribute_descriptions = GlyphInstance::getAttributeDescriptions();

    glyph_pipeline = new GpuPipeline(
        gpu, glyph_pipeline_layout, renderer->getCompositePass(), 0,
        glyphs->getVertexShader(), glyphs->getFragmentShader(), vertex_bindings,
        attribute_descriptions);
  }
}

OverlayPass::~OverlayPass() {
  log_zone;

  if (debug_vertex_shader != nullptr) delete debug_vertex_shader;
  if (debug_fragment_shader != nullptr) delete debug_fragment_shader;
  if (debug_pipeline != nullptr) delete debug_pipeline;
  if (debug_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, debug_pipeline_layout, nullptr);
  if (glyph_pipeline != nullptr) delete glyph_pipeline;
  if (glyph_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, glyph_pipeline_layout, nullptr);
  if (glyph_set_layout != nullptr) delete glyph_set_layout;
}

void OverlayPass::createFrameData(uint32_t frame_count) {
  log_zone;

  frame_data.resize(frame_count);

  for (auto& frame : frame_data) {
    frame.debug_vertices = new GpuVector(gpu, sizeof(DebugDrawVertex),
                                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    frame.debug_indices = new GpuVector(gpu, sizeof(DebugDrawIndex),
                                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    frame.glyph_instances = new GpuVector(gpu, sizeof(GlyphInstance),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  }
}

void OverlayPass::destroyFrameData() {
  log_zone;

  for (auto& frame : frame_data) {
    if (frame.debug_vertices != nullptr) delete frame.debug_vertices;
    if (frame.debug_indices != nullptr) delete frame.debug_indices;
    if (frame.glyph_instances != nullptr) delete frame.glyph_instances;
  }
}

void OverlayPass::allocateDescriptors(uint32_t frame_index,
                                      GpuDescriptorPool* descriptor_pool) {
  log_zone;

  auto& frame = frame_data[frame_index];

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
    auto transform_view = world->registry.view<TransformComponent>();

    for (auto& e : transform_view) {
      glm::mat4 transform = transform_view.get(e).getWorldTransform();

      {
        // Draw X line

        DebugDrawVertex vertex1{};
        vertex1.position = transform * glm::vec4(1.0, 0.0, 0.0, 1.0);
        vertex1.color = glm::vec3(1.0, 0.0, 0.0);

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{};
        vertex2.position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
        vertex2.color = glm::vec3(1.0, 0.0, 0.0);

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        // Draw Y line

        DebugDrawVertex vertex1{};
        vertex1.position = transform * glm::vec4(0.0, 1.0, 0.0, 1.0);
        vertex1.color = glm::vec3(0.0, 1.0, 0.0);

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{};
        vertex2.position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
        vertex2.color = glm::vec3(0.0, 1.0, 0.0);

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        // Draw Z line

        DebugDrawVertex vertex1{};
        vertex1.position = transform * glm::vec4(0.0, 0.0, 1.0, 1.0);
        vertex1.color = glm::vec3(0.0, 0.0, 1.0);

        frame.debug_vertices->writeElement(vertex_count, vertex1);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;

        DebugDrawVertex vertex2{};
        vertex2.position = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
        vertex2.color = glm::vec3(0.0, 0.0, 1.0);

        frame.debug_vertices->writeElement(vertex_count, vertex2);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }
    }
  }

  if (cvars->get<BoolCVar>("draw_lights")) {
    auto point_lights_view = world->registry.view<PointLightComponent>();

    for (auto e : point_lights_view) {
      auto& point_light = point_lights_view.get(e);

      PointLightUniform uniform;
      point_light.getUniform(&uniform);
      glm::vec3 position =
          glm::vec3(uniform.position.x, uniform.position.y, uniform.position.z);

      {
        DebugDrawVertex vertex{};
        vertex.position = position + glm::vec3(0.0, 0.1, 0.0);
        vertex.color = glm::vec3(1.0);

        frame.debug_vertices->writeElement(vertex_count, vertex);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }

      {
        DebugDrawVertex vertex{};
        vertex.position = position;
        vertex.color = glm::vec3(1.0);

        frame.debug_vertices->writeElement(vertex_count, vertex);
        frame.debug_indices->writeElement(frame.index_count, vertex_count);
        vertex_count++;
        frame.index_count++;
      }
    }
  }
}

void OverlayPass::render(uint32_t frame_index, VkCommandBuffer command_buffer,
                         const GpuDescriptorSet* viewport_descriptor) {
  log_zone;

  auto& frame = frame_data[frame_index];

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

  {
    log_zone_named("Render glyphs");

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
      depth_state.test_enable = GraphicsState::BoolFlag::False;
      depth_state.write_enable = GraphicsState::BoolFlag::False;
      depth_state.compare_op = GraphicsState::CompareOp::Always;
      graphics_state.depth_state = depth_state;

      glyph_pipeline->cmdBind(command_buffer, graphics_state);
    }

    viewport_descriptor->cmdBind(command_buffer, glyph_pipeline_layout, 0);
    frame.glyph_descriptor->cmdBind(command_buffer, glyph_pipeline_layout, 1);

    VkBuffer vertex_buffers[] = {frame.glyph_instances->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdDraw(command_buffer, 4, frame.glyph_count, 0, 0);
  }
}

}  // namespace mondradiko
