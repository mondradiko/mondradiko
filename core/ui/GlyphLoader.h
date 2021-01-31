// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuPipeline.h"
#include "lib/include/glm_headers.h"
#include "lib/include/msdfgen_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class GpuBuffer;
class GpuImage;
class GpuShader;

struct GlyphUniform {
  glm::vec2 atlas_coords[4];
  glm::vec2 glyph_coords[4];
};

struct GlyphInstance {
  glm::vec2 position;
  uint32_t glyph_index;

  static GpuPipeline::VertexBindings getVertexBindings() {
    GpuPipeline::VertexBindings bindings(1);

    bindings[0] = {.binding = 0,
                   .stride = sizeof(GlyphInstance),
                   .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE};

    return bindings;
  }

  static GpuPipeline::AttributeDescriptions getAttributeDescriptions() {
    GpuPipeline::AttributeDescriptions descriptions(2);

    descriptions[0] = {.location = 0,
                       .binding = 0,
                       .format = VK_FORMAT_R32G32_SFLOAT,
                       .offset = offsetof(GlyphInstance, position)};

    descriptions[1] = {.location = 1,
                       .binding = 0,
                       .format = VK_FORMAT_R32_UINT,
                       .offset = offsetof(GlyphInstance, glyph_index)};

    return descriptions;
  }
};

using GlyphString = std::vector<GlyphInstance>;

class GlyphLoader {
 public:
  static void initCVars(CVarScope*);

  GlyphLoader(const CVarScope*, GpuInstance*);
  ~GlyphLoader();

  VkSampler getSampler() const { return sdf_sampler; }
  const GpuImage* getAtlas() const { return atlas_image; }
  const GpuBuffer* getGlyphs() const { return glyph_buffer; }
  const GpuShader* getVertexShader() const { return vertex_shader; }
  const GpuShader* getFragmentShader() const { return fragment_shader; }

  void drawString(GlyphString*, const std::string&) const;

 private:
  const CVarScope* cvars;
  GpuInstance* gpu;

  FT_Library freetype;
  FT_Face font_face;
  msdfgen::FontHandle* sdf_font = nullptr;

  std::unordered_map<msdfgen::unicode_t, uint32_t> character_map;

  VkSampler sdf_sampler = VK_NULL_HANDLE;
  GpuImage* atlas_image = nullptr;
  GpuBuffer* glyph_buffer = nullptr;
  GpuShader* vertex_shader = nullptr;
  GpuShader* fragment_shader = nullptr;
};

}  // namespace mondradiko
