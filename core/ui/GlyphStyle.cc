// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/GlyphStyle.h"

namespace mondradiko {
namespace core {

GlyphStyle::GlyphStyle(GlyphLoader* glyphs) : glyphs(glyphs) {
  _uniform.transform = glm::mat4(1.0);
}

GlyphStyle::~GlyphStyle() {}

void GlyphStyle::setTransform(const glm::mat4& transform) {
  _uniform.transform = transform;
}

void GlyphStyle::drawString(GlyphString* glyph_string, uint32_t style_index,
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

    uint32_t glyph_index = glyphs->getGlyphIndex(c);

    GlyphInstance glyph;
    glyph.position = glm::vec2(cursor, line);
    glyph.style_index = style_index;
    glyph.glyph_index = glyph_index;

    glyph_string->push_back(glyph);
    cursor += 0.25;

    if (cursor >= 10.0) {
      cursor = 0.0;
      line -= 0.25;
    }
  }
}

}  // namespace core
}  // namespace mondradiko
