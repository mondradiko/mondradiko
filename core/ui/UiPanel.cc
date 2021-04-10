// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiPanel.h"

#include <cmath>

#include "core/scripting/ScriptEnvironment.h"
#include "core/ui/GlyphLoader.h"
#include "core/ui/GlyphStyle.h"

namespace mondradiko {
namespace core {

UiPanel::UiPanel(GlyphLoader* glyphs, ScriptEnvironment* scripts)
    : glyphs(glyphs), scripts(scripts) {
  _color = glm::vec4(0.0, 0.0, 0.0, 0.9);
  _position = glm::vec3(4.0, 1.25, 0.0);
  _orientation =
      glm::angleAxis(static_cast<float>(-M_PI_2), glm::vec3(0.0, 1.0, 0.0));
  _size = glm::vec2(0.8, 0.5);

  _style = new GlyphStyle(glyphs);
  _style->setTransform(
      getPlaneTransform() *
      glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(-0.45 * _size.x,
                                                          0.45 * _size.y, 0.0)),
                 glm::vec3(0.075)));

  _object_id = scripts->storeInRegistry(this);
}

UiPanel::~UiPanel() {
  scripts->removeFromRegistry(_object_id);
  if (_style != nullptr) delete _style;
}

void UiPanel::update(double dt) {}

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
  panel_uniform->color = _color;
}

UiPanel::StyleList UiPanel::getStyles() {
  StyleList styles{_style};
  return styles;
}

wasm_trap_t* UiPanel::getWidth(ScriptEnvironment*, const wasm_val_t[],
                               wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.x;
  return nullptr;
}

wasm_trap_t* UiPanel::getHeight(ScriptEnvironment*, const wasm_val_t[],
                                wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.x;
  return nullptr;
}

wasm_trap_t* UiPanel::setSize(ScriptEnvironment*, const wasm_val_t args[],
                              wasm_val_t results[]) {
  _size.x = args[1].of.f64;
  _size.y = args[2].of.f64;
  return nullptr;
}

wasm_trap_t* UiPanel::setColor(ScriptEnvironment*, const wasm_val_t args[],
                               wasm_val_t results[]) {
  _color.r = args[1].of.f64;
  _color.g = args[2].of.f64;
  _color.b = args[3].of.f64;
  _color.a = args[4].of.f64;

  return nullptr;
}

}  // namespace core
}  // namespace mondradiko
