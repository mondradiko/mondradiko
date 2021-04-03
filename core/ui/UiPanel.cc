// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiPanel.h"

#include "core/scripting/ScriptEnvironment.h"
#include "core/ui/GlyphLoader.h"

namespace mondradiko {
namespace core {

UiPanel::UiPanel(GlyphLoader* glyphs, ScriptEnvironment* scripts)
    : glyphs(glyphs), scripts(scripts) {
  _position = glm::vec3(4.0, 2.0, 0.0);
  _orientation = glm::quat();
  _size = glm::vec2(0.4, 0.3);
}

UiPanel::~UiPanel() {}

void UiPanel::writeUniform(PanelUniform* panel_uniform) {
  auto scale = glm::scale(glm::mat4(1.0), glm::vec3(_size.x, _size.y, 0.0));
  auto translate = glm::translate(glm::mat4(1.0), _position);
  auto rotate = glm::mat4(_orientation);

  panel_uniform->transform = translate * rotate * scale;
  panel_uniform->color = glm::vec4(0.0, 0.0, 0.0, 0.9);
}

}  // namespace core
}  // namespace mondradiko
