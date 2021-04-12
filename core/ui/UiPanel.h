// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/ScriptObject.h"
#include "lib/include/glm_headers.h"
#include "lib/include/wasm_headers.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GlyphLoader;
class GlyphStyle;
class ScriptEnvironment;
class World;

struct PanelUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

class UiPanel : public DynamicScriptObject<UiPanel> {
 public:
  UiPanel(GlyphLoader*, ScriptEnvironment*);
  ~UiPanel();

  void update(double);

  glm::mat4 getPlaneTransform();
  glm::mat4 getTrsTransform();
  void writeUniform(PanelUniform*);

  using StyleList = types::vector<GlyphStyle*>;
  const StyleList& getStyles() { return _styles; }

  //
  // Scripting methods
  //
  wasm_trap_t* getWidth(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getHeight(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setSize(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setColor(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* createGlyphStyle(ScriptEnvironment*, const wasm_val_t[],
                                wasm_val_t[]);

 private:
  GlyphLoader* glyphs;

  StyleList _styles;

  glm::vec4 _color;
  glm::vec3 _position;
  glm::quat _orientation;
  glm::vec2 _size;
};

}  // namespace core
}  // namespace mondradiko
