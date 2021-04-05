// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/GlyphLoader.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <lib/third_party/stb_rect_pack.h>

#include "core/cvars/CVarScope.h"
#include "core/cvars/FloatCVar.h"
#include "core/cvars/StringCVar.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuShader.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"
#include "shaders/glyph.frag.h"
#include "shaders/glyph.vert.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

void GlyphLoader::initCVars(CVarScope* cvars) {
  CVarScope* glyphs = cvars->addChild("glyphs");

  glyphs->addValue<StringCVar>("font_path");
  glyphs->addValue<FloatCVar>("sdf_scale", 1.0, 4.0);
  glyphs->addValue<FloatCVar>("sdf_border", 1.0, 10.0);
  glyphs->addValue<FloatCVar>("sdf_range", 1.0, 8.0);
}

GlyphLoader::GlyphLoader(const CVarScope* _cvars, Renderer* renderer)
    : cvars(_cvars->getChild("glyphs")),
      gpu(renderer->getGpu()),
      renderer(renderer) {
  FT_Error error = FT_Init_FreeType(&freetype);
  if (error) {
    log_ftl("Failed to initialize FreeType");
  }

  const char* font_path = cvars->get<StringCVar>("font_path").c_str();

  error = FT_New_Face(freetype, font_path, 0, &font_face);
  if (error) {
    log_ftl_fmt("Failed to load font %s", font_path);
  }

  sdf_font = msdfgen::adoptFreetypeFont(font_face);
  if (sdf_font == nullptr) {
    log_ftl_fmt("msdfgen failed to adopt font %s", font_path);
  }

  types::vector<msdfgen::unicode_t> characters;

  for (char c = 33; c <= 126; c++) {
    characters.push_back(c);
  }

  double sdf_scale = cvars->get<FloatCVar>("sdf_scale");
  double sdf_border = cvars->get<FloatCVar>("sdf_border");
  double sdf_range = cvars->get<FloatCVar>("sdf_range");

  types::vector<msdfgen::Shape> glyphs(characters.size());
  types::vector<stbrp_rect> glyph_rects(characters.size());

  for (uint32_t i = 0; i < glyphs.size(); i++) {
    auto& shape = glyphs[i];

    msdfgen::GlyphIndex glyph_index;

    char c = characters[i];
    if (!msdfgen::getGlyphIndex(glyph_index, sdf_font, c)) {
      log_err_fmt("No glyph for character %c", c);
      continue;
    }
    character_map.emplace(c, i);

    if (!msdfgen::loadGlyph(shape, sdf_font, glyph_index)) {
      log_ftl("msdfgen failed to load glyph");
    }

    shape.normalize();
    msdfgen::edgeColoringSimple(shape, 3.0);

    msdfgen::Shape::Bounds bounds = shape.getBounds(sdf_border);

    auto& rect = glyph_rects[i];
    rect.id = i;
    rect.w = round((bounds.r - bounds.l) * sdf_scale);
    rect.h = round((bounds.t - bounds.b) * sdf_scale);
  }

  bool packed_successfully = false;
  uint32_t atlas_width = 256;
  uint32_t atlas_height = 256;
  double atlas_scale_base = 2;

  const uint32_t MAX_ATLAS_SIZE = 1024;

  while (!packed_successfully && atlas_width < MAX_ATLAS_SIZE &&
         atlas_height < MAX_ATLAS_SIZE) {
    stbrp_context rp_context;
    int num_nodes = atlas_width;
    stbrp_node* nodes = new stbrp_node[num_nodes];
    stbrp_init_target(&rp_context, atlas_width, atlas_height, nodes, num_nodes);
    stbrp_pack_rects(&rp_context, glyph_rects.data(), glyph_rects.size());
    delete[] nodes;

    packed_successfully = true;
    for (auto& rect : glyph_rects) {
      if (rect.was_packed == 0) {
        atlas_width *= atlas_scale_base;
        atlas_height *= atlas_scale_base;
        packed_successfully = false;
        break;
      }
    }
  }

  if (!packed_successfully) {
    log_ftl("Failed to pack glyph atlas");
  }

  atlas_image =
      new GpuImage(gpu, VK_FORMAT_R8G8B8A8_UNORM, atlas_width, atlas_height, 1,
                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                   VMA_MEMORY_USAGE_GPU_ONLY);

  types::vector<GlyphUniform> glyph_data(glyph_rects.size());
  glyph_buffer = new GpuBuffer(
      gpu, glyph_data.size() * sizeof(glyph_data[0]),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  unsigned char* atlas_data = new unsigned char[atlas_width * atlas_height * 4];

  for (auto& rect : glyph_rects) {
    auto& glyph = glyphs[rect.id];

    msdfgen::Shape::Bounds bounds = glyph.getBounds(sdf_border);

    {
      double texture_width = atlas_width;
      double texture_height = atlas_height;

      double width = (rect.w - 2) / texture_width;
      double height = (rect.h - 2) / texture_height;

      double left = (rect.x + 1) / texture_width;
      double bottom = (rect.y + 1) / texture_height;
      double right = left + width;
      double top = bottom + height;

      auto& glyph_storage = glyph_data[rect.id];
      glyph_storage.atlas_coords[0] = glm::vec2(left, top);
      glyph_storage.atlas_coords[1] = glm::vec2(right, top);
      glyph_storage.atlas_coords[2] = glm::vec2(left, bottom);
      glyph_storage.atlas_coords[3] = glm::vec2(right, bottom);

      glyph_storage.glyph_coords[0] = glm::vec2(-width, height);
      glyph_storage.glyph_coords[1] = glm::vec2(width, height);
      glyph_storage.glyph_coords[2] = glm::vec2(-width, -height);
      glyph_storage.glyph_coords[3] = glm::vec2(width, -height);
    }

    {
      msdfgen::Vector2 translate(-bounds.l, -bounds.b);
      msdfgen::Bitmap<float, 4> msdf(rect.w, rect.h);
      msdfgen::generateMTSDF(msdf, glyph, sdf_range, sdf_scale, translate);

      for (uint32_t x = 0; x < rect.w; x++) {
        for (uint32_t y = 0; y < rect.h; y++) {
          float* src = msdf(x, y);
          unsigned char* dst =
              &atlas_data[(y + rect.y) * atlas_width * 4 + (x + rect.x) * 4];

          dst[0] = msdfgen::pixelFloatToByte(src[0]);
          dst[1] = msdfgen::pixelFloatToByte(src[1]);
          dst[2] = msdfgen::pixelFloatToByte(src[2]);
          dst[3] = msdfgen::pixelFloatToByte(src[3]);
        }
      }
    }
  }

  atlas_image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  renderer->transferDataToImage(atlas_image, atlas_data);
  atlas_image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  delete[] atlas_data;

  renderer->transferDataToBuffer(glyph_buffer, 0, glyph_data.data(),
                                 glyph_data.size() * sizeof(glyph_data[0]));

  {
    log_zone_named("Create SDF sampler");

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.mipLodBias = 0.0f;
    // TODO(marceline-cramer) Anisotropy support
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;

    if (vkCreateSampler(gpu->device, &sampler_info, nullptr, &sdf_sampler) !=
        VK_SUCCESS) {
      log_ftl("Failed to create texture sampler.");
    }
  }

  {
    log_zone_named("Create shaders");

    vertex_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_glyph_vert,
                      sizeof(shaders_glyph_vert));
    fragment_shader =
        new GpuShader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_glyph_frag,
                      sizeof(shaders_glyph_frag));
  }
}

GlyphLoader::~GlyphLoader() {
  if (vertex_shader != nullptr) delete vertex_shader;
  if (fragment_shader != nullptr) delete fragment_shader;
  if (sdf_sampler != VK_NULL_HANDLE)
    vkDestroySampler(gpu->device, sdf_sampler, nullptr);
  if (glyph_buffer != nullptr) delete glyph_buffer;
  if (atlas_image != nullptr) delete atlas_image;
  if (sdf_font != nullptr) msdfgen::destroyFont(sdf_font);
  FT_Done_Face(font_face);
  FT_Done_FreeType(freetype);
}

void GlyphLoader::drawString(GlyphString* glyph_string,
                             const types::string& text) const {
  double cursor = 0.0;
  double line = 0.0;

  for (uint32_t i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c == ' ') {
      cursor += 0.25;
      continue;
    } else if (c == '\n') {
      cursor = 0.0;
      line -= 0.25;
      continue;
    }

    uint32_t glyph_index = 0;

    auto iter = character_map.find(c);
    if (iter != character_map.end()) {
      glyph_index = iter->second;
    } else {
      cursor += 0.25;
      continue;
    }

    GlyphInstance glyph;
    glyph.position = glm::vec2(cursor, line);
    glyph.glyph_index = glyph_index;

    glyph_string->push_back(glyph);
    cursor += 0.25;
  }
}

}  // namespace core
}  // namespace mondradiko
