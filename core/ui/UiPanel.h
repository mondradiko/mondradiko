// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GlyphLoader;
class ScriptEnvironment;

struct PanelUniform {
  glm::mat4 transform;
  glm::vec4 color;
};

class UiPanel {
 public:
  UiPanel(GlyphLoader*, ScriptEnvironment*);
  ~UiPanel();

  void writeUniform(PanelUniform*);

 private:
  GlyphLoader* glyphs;
  ScriptEnvironment* scripts;

  glm::vec3 _position;
  glm::quat _orientation;
  glm::vec2 _size;
};

}  // namespace core
}  // namespace mondradiko
