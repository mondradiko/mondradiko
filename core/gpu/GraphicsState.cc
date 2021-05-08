// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GraphicsState.h"

#include "log/log.h"

namespace mondradiko {
namespace core {

#define HANDLE_INVALID_FLAGS(state, ret)    \
  log_ftl("Invalid GraphicsState " #state); \
  return ret

VkBool32 GraphicsState::CreateVkBool(BoolFlag bool_flag) {
  switch (bool_flag) {
    case GraphicsState::BoolFlag::False:
      return VK_FALSE;
    case GraphicsState::BoolFlag::True:
      return VK_TRUE;
    default:
      HANDLE_INVALID_FLAGS(bool_flag, VK_FALSE);
  }
}

VkCompareOp GraphicsState::CreateVkCompareOp(CompareOp compare_op) {
  switch (compare_op) {
    case GraphicsState::CompareOp::Never:
      return VK_COMPARE_OP_NEVER;
    case GraphicsState::CompareOp::Less:
      return VK_COMPARE_OP_LESS;
    case GraphicsState::CompareOp::Equal:
      return VK_COMPARE_OP_EQUAL;
    case GraphicsState::CompareOp::LessOrEqual:
      return VK_COMPARE_OP_LESS_OR_EQUAL;
    case GraphicsState::CompareOp::Greater:
      return VK_COMPARE_OP_GREATER;
    case GraphicsState::CompareOp::NotEqual:
      return VK_COMPARE_OP_NOT_EQUAL;
    case GraphicsState::CompareOp::GreaterOrEqual:
      return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case GraphicsState::CompareOp::Always:
      return VK_COMPARE_OP_ALWAYS;
    default:
      HANDLE_INVALID_FLAGS(compare_op, VK_COMPARE_OP_MAX_ENUM);
  }
}

VkPrimitiveTopology GraphicsState::CreateVkPrimitiveTopology(
    PrimitiveTopology primitive_topology) {
  switch (primitive_topology) {
    case GraphicsState::PrimitiveTopology::PointList:
      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case GraphicsState::PrimitiveTopology::LineList:
      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case GraphicsState::PrimitiveTopology::LineStrip:
      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case GraphicsState::PrimitiveTopology::TriangleList:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case GraphicsState::PrimitiveTopology::TriangleStrip:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case GraphicsState::PrimitiveTopology::TriangleFan:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    default:
      HANDLE_INVALID_FLAGS(primitive_topology, VK_PRIMITIVE_TOPOLOGY_MAX_ENUM);
  }
}

VkPolygonMode GraphicsState::CreateVkPolygonMode(PolygonMode polygon_mode) {
  switch (polygon_mode) {
    case GraphicsState::PolygonMode::Fill:
      return VK_POLYGON_MODE_FILL;
    case GraphicsState::PolygonMode::Line:
      return VK_POLYGON_MODE_LINE;
    case GraphicsState::PolygonMode::Point:
      return VK_POLYGON_MODE_POINT;
    default:
      HANDLE_INVALID_FLAGS(polygon_mode, VK_POLYGON_MODE_MAX_ENUM);
  }
}

VkCullModeFlags GraphicsState::CreateVkCullMode(CullMode cull_mode) {
  switch (cull_mode) {
    case GraphicsState::CullMode::None:
      return VK_CULL_MODE_NONE;
    case GraphicsState::CullMode::Front:
      return VK_CULL_MODE_FRONT_BIT;
    case GraphicsState::CullMode::Back:
      return VK_CULL_MODE_BACK_BIT;
    case GraphicsState::CullMode::Both:
      return VK_CULL_MODE_FRONT_AND_BACK;
    default:
      HANDLE_INVALID_FLAGS(cull_mode, VK_CULL_MODE_FLAG_BITS_MAX_ENUM);
  }
}

VkSampleCountFlagBits GraphicsState::CreateVkSampleCount(
    SampleCount sample_count) {
  switch (sample_count) {
    case (1 << 0):
      return VK_SAMPLE_COUNT_1_BIT;
    case (1 << 1):
      return VK_SAMPLE_COUNT_2_BIT;
    case (1 << 2):
      return VK_SAMPLE_COUNT_4_BIT;
    case (1 << 3):
      return VK_SAMPLE_COUNT_8_BIT;
    case (1 << 4):
      return VK_SAMPLE_COUNT_16_BIT;
    case (1 << 5):
      return VK_SAMPLE_COUNT_32_BIT;
    case (1 << 6):
      return VK_SAMPLE_COUNT_64_BIT;
    default:
      HANDLE_INVALID_FLAGS(sample_count, VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM);
  }
}

// VkStencilOp createVkStencilOp(GraphicsState::StencilOp stencil_op) {}

VkPipelineInputAssemblyStateCreateInfo
GraphicsState::createVkInputAssemblyState() const {
  const auto& state = input_assembly_state;

  VkPipelineInputAssemblyStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology = CreateVkPrimitiveTopology(state.primitive_topology);
  info.primitiveRestartEnable = CreateVkBool(state.primitive_restart_enable);

  return info;
}

VkPipelineRasterizationStateCreateInfo
GraphicsState::createVkRasterizationState() const {
  VkPipelineRasterizationStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.depthClampEnable = VK_FALSE;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.polygonMode = CreateVkPolygonMode(rasterization_state.polygon_mode);
  info.cullMode = CreateVkCullMode(rasterization_state.cull_mode);
  info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  info.depthBiasEnable = VK_FALSE;
  info.lineWidth = 1.0f;

  return info;
}

VkPipelineMultisampleStateCreateInfo GraphicsState::createVkMultisampleState()
    const {
  const auto& state = multisample_state;

  VkPipelineMultisampleStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.minSampleShading = 1.0f;
  info.pSampleMask = nullptr;
  info.rasterizationSamples = CreateVkSampleCount(state.rasterization_samples);
  info.sampleShadingEnable = CreateVkBool(state.sample_shading_enable);
  info.alphaToCoverageEnable = CreateVkBool(state.alpha_to_coverage_enable);
  info.alphaToOneEnable = CreateVkBool(state.alpha_to_one_enable);

  return info;
}

VkPipelineDepthStencilStateCreateInfo GraphicsState::createVkDepthStencilState()
    const {
  VkPipelineDepthStencilStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.depthTestEnable = CreateVkBool(depth_state.test_enable);
  info.depthWriteEnable = CreateVkBool(depth_state.write_enable);
  info.depthCompareOp = CreateVkCompareOp(depth_state.compare_op);
  info.depthBoundsTestEnable = VK_FALSE;
  info.stencilTestEnable = VK_FALSE;

  return info;
}

VkPipelineColorBlendAttachmentState GraphicsState::createVkColorBlendState()
    const {
  VkPipelineColorBlendAttachmentState info{};

  info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  switch (color_blend_state.blend_mode) {
    case GraphicsState::BlendMode::Opaque: {
      info.blendEnable = VK_FALSE;
      break;
    }

    case GraphicsState::BlendMode::AlphaBlend: {
      info.blendEnable = VK_TRUE;

      info.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      info.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      info.colorBlendOp = VK_BLEND_OP_ADD;
      info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      info.alphaBlendOp = VK_BLEND_OP_ADD;

      break;
    }

    case GraphicsState::BlendMode::AlphaPremultiplied: {
      info.blendEnable = VK_TRUE;

      info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      info.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      info.colorBlendOp = VK_BLEND_OP_ADD;
      info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      info.alphaBlendOp = VK_BLEND_OP_ADD;

      break;
    }

    default: {
      log_ftl("Invalid GraphicsState");
      break;
    }
  }

  return info;
}

GraphicsState GraphicsState::CreateGenericOpaque() {
  GraphicsState gs{};

  InputAssemblyState input_assembly{};
  input_assembly.primitive_topology = PrimitiveTopology::TriangleList;
  input_assembly.primitive_restart_enable = BoolFlag::False;
  gs.input_assembly_state = input_assembly;

  RasterizatonState rasterization{};
  rasterization.polygon_mode = PolygonMode::Fill;
  rasterization.cull_mode = CullMode::None;
  gs.rasterization_state = rasterization;

  MultisampleState multisample{};
  multisample.rasterization_samples = 1;
  multisample.sample_shading_enable = BoolFlag::False;
  multisample.alpha_to_coverage_enable = BoolFlag::False;
  multisample.alpha_to_one_enable = BoolFlag::False;
  gs.multisample_state = multisample;

  DepthState depth{};
  depth.test_enable = BoolFlag::True;
  depth.write_enable = BoolFlag::True;
  depth.compare_op = CompareOp::Less;
  gs.depth_state = depth;

  ColorBlendState color_blend{};
  color_blend.blend_mode = BlendMode::Opaque;
  gs.color_blend_state = color_blend;

  return gs;
}

}  // namespace core
}  // namespace mondradiko
