// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/object/DynamicScriptObject.h"
#include "core/ui/GlyphLoader.h"
#include "lib/include/wasm_headers.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ScriptEnvironment;
class UiPanel;
class World;

struct GlyphStyleUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

using GlyphString = types::vector<GlyphInstance>;
using GlyphStyleList = types::vector<class GlyphStyle*>;

class GlyphStyle : public DynamicScriptObject<GlyphStyle> {
 public:
  explicit GlyphStyle(GlyphLoader*, ScriptEnvironment*, UiPanel*);
  ~GlyphStyle();

  void drawString(GlyphString*, uint32_t, const types::string&) const;
  GlyphStyleUniform getUniform() const;

  //
  // Scripting methods
  //
  wasm_trap_t* setOffset(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setScale(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setColor(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);

 private:
  GlyphLoader* glyphs;

  UiPanel* _panel;
  glm::vec4 _color;
  glm::vec2 _offset;
  double _scale;
};

}  // namespace core
}  // namespace mondradiko
