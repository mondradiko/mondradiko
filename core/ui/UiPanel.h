// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/object/DynamicScriptObject.h"
#include "lib/include/glm_headers.h"
#include "lib/include/wasm_headers.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GlyphLoader;
class GlyphStyle;
class UiScript;
class World;

struct PanelUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

class UiPanel : public DynamicScriptObject<UiPanel> {
 public:
  UiPanel(GlyphLoader*, UiScript*, const types::string&);
  ~UiPanel();

  void update(double);
  void handleMessage(const types::string&);

  glm::mat4 getPlaneTransform();
  glm::mat4 getTrsTransform();
  void writeUniform(PanelUniform*);

  using StyleList = types::vector<GlyphStyle*>;
  const StyleList& getStyles() { return _styles; }

  //
  // Scripting methods
  //
  wasm_trap_t* getWidth(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getHeight(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setSize(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setColor(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* createGlyphStyle(ScriptInstance*, const wasm_val_t[],
                                wasm_val_t[]);

 private:
  GlyphLoader* glyphs;
  UiScript* ui_script;

  types::string _impl;
  uint32_t _this_ptr;

  StyleList _styles;

  glm::vec4 _color;
  glm::vec3 _position;
  glm::quat _orientation;
  glm::vec2 _size;
};

}  // namespace core
}  // namespace mondradiko
