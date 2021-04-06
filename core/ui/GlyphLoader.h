// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"
#include "lib/include/msdfgen_headers.h"
#include "types/containers/unordered_map.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CVarScope;
class GpuBuffer;
class GpuImage;
class GpuShader;
class Renderer;

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

class GlyphLoader {
 public:
  static void initCVars(CVarScope*);

  GlyphLoader(const CVarScope*, Renderer*);
  ~GlyphLoader();

  VkSampler getSampler() const { return sdf_sampler; }
  const GpuImage* getAtlas() const { return atlas_image; }
  const GpuBuffer* getGlyphs() const { return glyph_buffer; }
  const GpuShader* getVertexShader() const { return vertex_shader; }
  const GpuShader* getFragmentShader() const { return fragment_shader; }

  uint32_t getGlyphIndex(msdfgen::unicode_t);

 private:
  const CVarScope* cvars;
  GpuInstance* gpu;
  Renderer* renderer;

  FT_Library freetype;
  FT_Face font_face;
  msdfgen::FontHandle* sdf_font = nullptr;

  types::unordered_map<msdfgen::unicode_t, uint32_t> character_map;

  VkSampler sdf_sampler = VK_NULL_HANDLE;
  GpuImage* atlas_image = nullptr;
  GpuBuffer* glyph_buffer = nullptr;
  GpuShader* vertex_shader = nullptr;
  GpuShader* fragment_shader = nullptr;
};

}  // namespace core
}  // namespace mondradiko
