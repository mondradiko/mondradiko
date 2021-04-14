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
    : DynamicScriptObject(scripts), glyphs(glyphs) {
  _color = glm::vec4(0.0, 0.0, 0.0, 0.9);
  _position = glm::vec3(4.0, 1.25, 0.0);
  _orientation =
      glm::angleAxis(static_cast<float>(-M_PI_2), glm::vec3(0.0, 1.0, 0.0));
  _size = glm::vec2(1.6, 1.0);
}

UiPanel::~UiPanel() {
  for (auto& style : _styles) {
    if (style != nullptr) delete style;
  }
}

void UiPanel::update(double dt) {}

glm::mat4 UiPanel::getPlaneTransform() {
  auto translate = glm::translate(glm::mat4(1.0), _position);
  auto rotate = glm::mat4(_orientation);
  return translate * rotate;
}

glm::mat4 UiPanel::getTrsTransform() {
  double fit = (_size.x < _size.y) ? _size.x : _size.y;
  auto scale = glm::scale(glm::mat4(1.0), glm::vec3(fit));
  return getPlaneTransform() * scale;
}

void UiPanel::writeUniform(PanelUniform* panel_uniform) {
  panel_uniform->transform = getTrsTransform();
  panel_uniform->color = _color;
}

wasm_trap_t* UiPanel::getWidth(const wasm_val_t[], wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.x;
  return nullptr;
}

wasm_trap_t* UiPanel::getHeight(const wasm_val_t[], wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.y;
  return nullptr;
}

wasm_trap_t* UiPanel::setSize(const wasm_val_t args[], wasm_val_t results[]) {
  _size.x = args[1].of.f64;
  _size.y = args[2].of.f64;
  return nullptr;
}

wasm_trap_t* UiPanel::setColor(const wasm_val_t args[], wasm_val_t results[]) {
  _color.r = args[1].of.f64;
  _color.g = args[2].of.f64;
  _color.b = args[3].of.f64;
  _color.a = args[4].of.f64;

  return nullptr;
}

wasm_trap_t* UiPanel::createGlyphStyle(const wasm_val_t[],
                                       wasm_val_t results[]) {
  GlyphStyle* new_style = new GlyphStyle(glyphs, scripts, this);
  _styles.push_back(new_style);

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_style->getObjectKey();

  return nullptr;
}

}  // namespace core
}  // namespace mondradiko
