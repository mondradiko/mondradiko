// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/GlyphLoader.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <lib/third_party/stb_rect_pack.h>

#include <vector>

#include "core/cvars/CVarScope.h"
#include "core/cvars/FloatCVar.h"
#include "core/cvars/StringCVar.h"
#include "log/log.h"

namespace mondradiko {

void GlyphLoader::initCVars(CVarScope* cvars) {
  CVarScope* glyphs = cvars->addChild("glyphs");

  glyphs->addValue<StringCVar>("font_path");
  glyphs->addValue<FloatCVar>("glyph_scale", 1.0, 4.0);
  glyphs->addValue<FloatCVar>("glyph_border", 0.0, 10.0);
  glyphs->addValue<FloatCVar>("sdf_range", 1.0, 8.0);
}

GlyphLoader::GlyphLoader(const CVarScope* _cvars)
    : cvars(_cvars->getChild("glyphs")) {
  FT_Error error = FT_Init_FreeType(&freetype);
  if (error) {
    log_ftl("Failed to initialize FreeType");
  }

  const char* font_path = cvars->get<StringCVar>("font_path").c_str();

  error = FT_New_Face(freetype, font_path, 0, &font_face);
  if (error) {
    log_ftl("Failed to load font %s", font_path);
  }

  sdf_font = msdfgen::adoptFreetypeFont(font_face);
  if (sdf_font == nullptr) {
    log_ftl("msdfgen failed to adopt font %s", font_path);
  }

  std::vector<msdfgen::unicode_t> characters;

  for (char c = 33; c <= 126; c++) {
    characters.push_back(c);
  }

  double glyph_scale = cvars->get<FloatCVar>("glyph_scale");
  double glyph_border = cvars->get<FloatCVar>("glyph_border");
  double sdf_range = cvars->get<FloatCVar>("sdf_range");

  std::vector<msdfgen::Shape> glyphs(characters.size());
  std::vector<stbrp_rect> glyph_rects(characters.size());

  for (uint32_t i = 0; i < glyphs.size(); i++) {
    auto& shape = glyphs[i];

    if (!msdfgen::loadGlyph(shape, sdf_font, characters[i])) {
      log_ftl("msdfgen failed to load glyph");
    }

    shape.normalize();
    msdfgen::edgeColoringSimple(shape, 3.0);

    msdfgen::Shape::Bounds bounds = shape.getBounds(glyph_border);

    auto& rect = glyph_rects[i];
    rect.id = i;
    rect.w = round((bounds.r - bounds.l) * glyph_scale);
    rect.h = round((bounds.t - bounds.b) * glyph_scale);
  }

  bool packed_successfully = false;
  uint32_t atlas_width = 32;
  uint32_t atlas_height = 32;
  double atlas_scale_base = sqrt(1.5);

  while (!packed_successfully) {
    stbrp_context rp_context;
    int num_nodes = atlas_width;
    stbrp_node* nodes = new stbrp_node[atlas_width];
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

  msdfgen::Bitmap<float, 3> atlas(atlas_width, atlas_height);

  for (auto& rect : glyph_rects) {
    auto& glyph = glyphs[rect.id];

    msdfgen::Shape::Bounds bounds = glyph.getBounds(glyph_border);
    msdfgen::Vector2 translate;
    translate.set(-bounds.l, -bounds.b);

    msdfgen::Bitmap<float, 3> msdf(rect.w, rect.h);
    msdfgen::generateMSDF(msdf, glyph, sdf_range, glyph_scale, translate);

    for (uint32_t x = 0; x < rect.w; x++) {
      for (uint32_t y = 0; y < rect.h; y++) {
        float* src = msdf(x, y);
        float* dst = atlas(x + rect.x, y + rect.y);

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
      }
    }
  }

  msdfgen::savePng(atlas, "glyph-atlas.png");
}

GlyphLoader::~GlyphLoader() {
  if (sdf_font != nullptr) msdfgen::destroyFont(sdf_font);
  FT_Done_Face(font_face);
  FT_Done_FreeType(freetype);
}

void GlyphLoader::loadGlyph(msdfgen::unicode_t character) {
  msdfgen::Shape shape;
  if (!msdfgen::loadGlyph(shape, sdf_font, 'A')) {
    log_ftl("msdfgen failed to load glyph");
  }

  shape.normalize();
  msdfgen::edgeColoringSimple(shape, 3.0);
}

}  // namespace mondradiko
