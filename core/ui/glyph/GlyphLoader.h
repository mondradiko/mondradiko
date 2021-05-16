// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <array>

#include "lib/include/msdfgen_headers.h"
#include "lib/include/vulkan_headers.h"
#include "types/containers/unordered_map.h"

namespace mondradiko {
namespace core {

// Forward declarations
class CVarScope;
class GpuBuffer;
class GpuImage;
class GpuInstance;
class GpuShader;
class Renderer;

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
