// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/glm_headers.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GlyphLoader;
class GlyphStyle;
class ScriptEnvironment;

struct PanelUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

class UiPanel {
 public:
  UiPanel(GlyphLoader*, ScriptEnvironment*);
  ~UiPanel();

  glm::mat4 getPlaneTransform();
  glm::mat4 getTrsTransform();
  void writeUniform(PanelUniform*);

  using StyleList = types::vector<GlyphStyle*>;
  StyleList getStyles();

 private:
  GlyphLoader* glyphs;
  ScriptEnvironment* scripts;

  GlyphStyle* _style = nullptr;

  glm::vec3 _position;
  glm::quat _orientation;
  glm::vec2 _size;
};

}  // namespace core
}  // namespace mondradiko
