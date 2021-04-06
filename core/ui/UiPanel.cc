// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiPanel.h"

#include "core/scripting/ScriptEnvironment.h"
#include "core/ui/GlyphLoader.h"
#include "core/ui/GlyphStyle.h"

namespace mondradiko {
namespace core {

UiPanel::UiPanel(GlyphLoader* glyphs, ScriptEnvironment* scripts)
    : glyphs(glyphs), scripts(scripts) {
  _position = glm::vec3(4.0, 2.0, 0.0);
  _orientation = glm::quat();
  _size = glm::vec2(0.4, 0.3);

  _style = new GlyphStyle(glyphs);
  _style->setTransform(
      getTrsTransform() *
      glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(-0.95, 0.95, 0.0)),
                 glm::vec3(0.2)));
}

UiPanel::~UiPanel() {
  if (_style != nullptr) delete _style;
}

glm::mat4 UiPanel::getPlaneTransform() {
  auto translate = glm::translate(glm::mat4(1.0), _position);
  auto rotate = glm::mat4(_orientation);
  return translate * rotate;
}

glm::mat4 UiPanel::getTrsTransform() {
  auto scale = glm::scale(glm::mat4(1.0), glm::vec3(_size.x, _size.y, 0.0));
  return getPlaneTransform() * scale;
}

void UiPanel::writeUniform(PanelUniform* panel_uniform) {
  panel_uniform->transform = getTrsTransform();
  panel_uniform->color = glm::vec4(0.0, 0.0, 0.0, 0.9);
}

UiPanel::StyleList UiPanel::getStyles() {
  StyleList styles{_style};
  return styles;
}

}  // namespace core
}  // namespace mondradiko
