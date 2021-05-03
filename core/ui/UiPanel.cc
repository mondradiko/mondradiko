// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiPanel.h"

#include <cmath>

#include "core/scripting/instance/UiScript.h"
#include "core/ui/GlyphLoader.h"
#include "core/ui/GlyphStyle.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

UiPanel::UiPanel(GlyphLoader* glyphs, UiScript* ui_script,
                 const types::string& impl)
    : DynamicScriptObject(ui_script->scripts),
      glyphs(glyphs),
      ui_script(ui_script),
      _impl(impl) {
  _color = glm::vec4(0.0, 0.0, 0.0, 0.9);
  _position = glm::vec3(4.0, 1.25, 0.0);
  _orientation =
      glm::angleAxis(static_cast<float>(-M_PI_2), glm::vec3(0.0, 1.0, 0.0));
  _size = glm::vec2(1.6, 1.0);

  wasm_val_t panel_arg;
  panel_arg.kind = WASM_I32;
  panel_arg.of.i32 = getObjectKey();

  if (ui_script->AS_construct(impl, &panel_arg, 1, &_this_ptr)) {
    ui_script->AS_pin(_this_ptr);
  } else {
    log_err_fmt("Failed to bind UI panel");
    _this_ptr = 0;
  }
}

UiPanel::~UiPanel() {
  ui_script->AS_unpin(_this_ptr);

  for (auto& style : _styles) {
    if (style != nullptr) delete style;
  }
}

void UiPanel::update(double dt) {
  types::string update_callback = _impl + "#update";
  if (!ui_script->hasCallback(update_callback)) {
    log_wrn_fmt("UI script does not export %s", update_callback.c_str());
    return;
  }

  std::array<wasm_val_t, 2> args;

  auto& this_arg = args[0];
  this_arg.kind = WASM_I32;
  this_arg.of.i32 = _this_ptr;

  auto& dt_arg = args[1];
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;

  ui_script->runCallback(update_callback, args.data(), args.size(), nullptr, 0);
}

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

wasm_trap_t* UiPanel::getWidth(ScriptInstance*, const wasm_val_t[],
                               wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.x;
  return nullptr;
}

wasm_trap_t* UiPanel::getHeight(ScriptInstance*, const wasm_val_t[],
                                wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _size.y;
  return nullptr;
}

wasm_trap_t* UiPanel::setSize(ScriptInstance*, const wasm_val_t args[],
                              wasm_val_t results[]) {
  _size.x = args[1].of.f64;
  _size.y = args[2].of.f64;
  return nullptr;
}

wasm_trap_t* UiPanel::setColor(ScriptInstance*, const wasm_val_t args[],
                               wasm_val_t results[]) {
  _color.r = args[1].of.f64;
  _color.g = args[2].of.f64;
  _color.b = args[3].of.f64;
  _color.a = args[4].of.f64;

  return nullptr;
}

wasm_trap_t* UiPanel::createGlyphStyle(ScriptInstance*, const wasm_val_t[],
                                       wasm_val_t results[]) {
  GlyphStyle* new_style = new GlyphStyle(glyphs, scripts, this);
  _styles.push_back(new_style);

  results[0].kind = WASM_I32;
  results[0].of.i32 = new_style->getObjectKey();

  return nullptr;
}

}  // namespace core
}  // namespace mondradiko
