// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

struct GlyphUniform {
  glm::vec2 atlas_coords[4];
  glm::vec2 glyph_coords[4];
};

struct GlyphInstance {
  glm::vec2 position;
  uint32_t style_index;
  uint32_t glyph_index;

  static GpuPipeline::VertexBindings getVertexBindings() {
    // VkVertexInputBindingDescription{binding, stride, inputRate}
    return {
        {0, sizeof(GlyphInstance), VK_VERTEX_INPUT_RATE_INSTANCE},
    };
  }

  static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
    // VkVertexInputAttributeDescription{location, binding, format, offset}
    return {{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(GlyphInstance, position)},
            {1, 0, VK_FORMAT_R32_UINT, offsetof(GlyphInstance, style_index)},
            {2, 0, VK_FORMAT_R32_UINT, offsetof(GlyphInstance, glyph_index)}};
  }
};

struct GlyphStyleUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

using GlyphString = types::vector<GlyphInstance>;
using GlyphStyleList = types::vector<class GlyphStyle*>;

}  // namespace core
}  // namespace mondradiko
