// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

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

class UiPanel {
 public:
  UiPanel(GlyphLoader*, ScriptEnvironment*);
  ~UiPanel();

  void update(double);

  glm::mat4 getPlaneTransform();
  glm::mat4 getTrsTransform();
  void writeUniform(PanelUniform*);

  uint32_t getScriptObject() { return _object_id; }

  using StyleList = types::vector<GlyphStyle*>;
  StyleList getStyles();

  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*, World*);

 private:
  GlyphLoader* glyphs;
  ScriptEnvironment* scripts;

  GlyphStyle* _style = nullptr;
  uint32_t _object_id;

  glm::vec4 _color;
  glm::vec3 _position;
  glm::quat _orientation;
  glm::vec2 _size;

  //
  // Scripting methods
  //
  wasm_trap_t* getWidth(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getHeight(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setSize(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* setColor(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
};

}  // namespace core
}  // namespace mondradiko
