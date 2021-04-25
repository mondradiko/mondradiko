// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/GlyphStyle.h"

#include "core/scripting/instance/ScriptInstance.h"
#include "core/ui/UiPanel.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

GlyphStyle::GlyphStyle(GlyphLoader* glyphs, ScriptEnvironment* scripts,
                       UiPanel* panel)
    : DynamicScriptObject(scripts), glyphs(glyphs), _panel(panel) {
  _color = glm::vec4(1.0);
  _offset = glm::vec2(-0.45);
  _scale = 0.15;
}

GlyphStyle::~GlyphStyle() {}

void GlyphStyle::drawString(GlyphString* glyph_string,
                            uint32_t style_index) const {
  const types::string& text = _text;

  double cursor = 0.0;
  double line = 0.0;

  for (uint32_t i = 0; i < _text.length(); i++) {
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

GlyphStyleUniform GlyphStyle::getUniform() const {
  GlyphStyleUniform ubo{};
  ubo.transform =
      _panel->getTrsTransform() *
      glm::translate(glm::mat4(1.0), glm::vec3(_offset.x, -_offset.y, 0.0)) *
      glm::scale(glm::mat4(1.0), glm::vec3(_scale));
  ubo.color = _color;
  return ubo;
}

wasm_trap_t* GlyphStyle::setOffset(ScriptInstance*, const wasm_val_t args[],
                                   wasm_val_t[]) {
  _offset.x = args[1].of.f64;
  _offset.y = args[2].of.f64;

  return nullptr;
}

wasm_trap_t* GlyphStyle::setScale(ScriptInstance*, const wasm_val_t args[],
                                  wasm_val_t[]) {
  _scale = args[1].of.f64;

  return nullptr;
}

wasm_trap_t* GlyphStyle::setColor(ScriptInstance*, const wasm_val_t args[],
                                  wasm_val_t[]) {
  _color.r = args[1].of.f64;
  _color.g = args[2].of.f64;
  _color.b = args[3].of.f64;
  _color.a = args[4].of.f64;

  return nullptr;
}

wasm_trap_t* GlyphStyle::setText(ScriptInstance* instance,
                                 const wasm_val_t args[], wasm_val_t[]) {
  types::string text;
  if (instance->AS_getString(args[1].of.i32, &text)) {
    _text = text;
  }

  return nullptr;
}

}  // namespace core
}  // namespace mondradiko
