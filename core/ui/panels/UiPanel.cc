// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/panels/UiPanel.h"

#include <cmath>

#include "core/scripting/instance/UiScript.h"
#include "core/ui/UiDrawList.h"
#include "core/ui/glyph/GlyphLoader.h"
#include "core/ui/glyph/GlyphStyle.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

UiPanel::UiPanel(GlyphLoader* glyphs, ScriptEnvironment* scripts)
    : DynamicScriptObject(scripts), glyphs(glyphs) {
  _color = glm::vec4(0.0, 0.0, 0.0, 0.9);
  _position = glm::vec3(1.0, 1.25, 0.0);
  _orientation =
      glm::angleAxis(static_cast<float>(-M_PI_2), glm::vec3(0.0, 1.0, 0.0));
  _size = glm::vec2(1.6, 1.0);
}

UiPanel::~UiPanel() {
  bindUiScript(nullptr, "");

  for (auto& style : _styles) {
    if (style != nullptr) delete style;
  }
}

void UiPanel::bindUiScript(UiScript* new_script,
                           const types::string& new_impl) {
  if (ui_script != nullptr) ui_script->AS_unpin(_this_ptr);

  if (new_script == nullptr) return;

  ui_script = new_script;
  _impl = new_impl;

  for (auto& style : _styles) {
    if (style != nullptr) delete style;
  }

  _styles.clear();

  wasm_val_t panel_arg;
  panel_arg.kind = WASM_I32;
  panel_arg.of.i32 = getObjectKey();

  if (ui_script->AS_construct(_impl, &panel_arg, 1, &_this_ptr)) {
    ui_script->AS_pin(_this_ptr);
  } else {
    log_err_fmt("Failed to bind UI panel");
    _this_ptr = 0;
  }
}

void UiPanel::update(double dt, UiDrawList* ui_draw) {
  _current_draw = ui_draw;

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

void UiPanel::runCallbackWithCoords(const types::string& method,
                                    const glm::vec2& coords) {
  types::string callback = _impl + "#" + method;
  if (!ui_script->hasCallback(callback)) {
    log_wrn_fmt("UI script does not export %s", callback.c_str());
    return;
  }

  std::array<wasm_val_t, 3> args;

  auto& this_arg = args[0];
  this_arg.kind = WASM_I32;
  this_arg.of.i32 = _this_ptr;

  auto& x_arg = args[1];
  x_arg.kind = WASM_F64;
  x_arg.of.f64 = coords.x;

  auto& y_arg = args[2];
  y_arg.kind = WASM_F64;
  y_arg.of.f64 = coords.y;

  ui_script->runCallback(callback, args.data(), args.size(), nullptr, 0);
}

void UiPanel::onHover(const glm::vec2& coords) {
  runCallbackWithCoords("onHover", coords);
}

void UiPanel::onSelect(const glm::vec2& coords) {
  runCallbackWithCoords("onSelect", coords);
}

void UiPanel::onDrag(const glm::vec2& coords) {
  runCallbackWithCoords("onDrag", coords);
}

void UiPanel::onDeselect(const glm::vec2& coords) {
  runCallbackWithCoords("onDeselect", coords);
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
  panel_uniform->transform = getPlaneTransform();
  panel_uniform->color = _color;
  panel_uniform->size = _size;
}

glm::mat4 UiPanel::getInverseTransform() {
  auto rotate = glm::transpose(glm::mat4(_orientation));
  return glm::translate(rotate, -_position);
}

double UiPanel::getPointDistance(const glm::vec3& position) {
  glm::vec3 normal = getNormal();
  double position_dot = glm::dot(normal, position - _position);
  return position_dot;
}

glm::vec3 UiPanel::getNormal() {
  return glm::mat4(_orientation) * glm::vec4(0.0, 0.0, 1.0, 1.0);
}

double UiPanel::getRayIntersectFactor(const glm::vec3& position,
                                      const glm::vec3& direction) {
  glm::vec3 normal = getNormal();

  double direction_dot = glm::dot(normal, -direction);

  // Catch NaNs
  if (direction_dot == 0.0) return -1.0;

  double factor = glm::dot(normal, position - _position) / direction_dot;

  // Discard the ray tail
  if (factor < 0.0) return -1.0;

  glm::vec2 coords = getRayIntersectCoords(position, direction);
  glm::vec2 abs_coords = glm::abs(coords) * glm::vec2(2.0);

  // Discard out-of-bounds coords
  if (abs_coords.x > _size.x || abs_coords.y > _size.y) return -1.0;

  return factor;
}

glm::vec2 UiPanel::getRayIntersectCoords(const glm::vec3& position,
                                         const glm::vec3& direction) {
  glm::vec3 normal = getNormal();

  double direction_dot = glm::dot(normal, -direction);
  double factor = glm::dot(normal, position - _position) / direction_dot;

  glm::vec3 world_coords = position + direction * glm::vec3(factor);
  glm::vec2 panel_coords = getInverseTransform() * glm::vec4(world_coords, 1.0);

  return panel_coords;
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

wasm_trap_t* UiPanel::drawTriangle(ScriptInstance*, const wasm_val_t args[],
                                   wasm_val_t[]) {
  glm::vec2 position1 = glm::vec2(args[1].of.f64, args[2].of.f64);
  glm::vec2 position2 = glm::vec2(args[3].of.f64, args[4].of.f64);
  glm::vec2 position3 = glm::vec2(args[5].of.f64, args[6].of.f64);
  glm::vec4 color = glm::vec4(args[7].of.f64, args[8].of.f64, args[9].of.f64,
                              args[10].of.f64);

  auto vertex1 = _current_draw->makeVertex(position1, color);
  auto vertex2 = _current_draw->makeVertex(position2, color);
  auto vertex3 = _current_draw->makeVertex(position3, color);

  _current_draw->drawTriangle(vertex1, vertex2, vertex3);

  return nullptr;
}

wasm_trap_t* UiPanel::drawCircle(ScriptInstance*, const wasm_val_t args[],
                                 wasm_val_t[]) {
  glm::vec2 center = glm::vec2(args[1].of.f64, args[2].of.f64);
  float radius = args[3].of.f64;
  glm::vec4 color =
      glm::vec4(args[4].of.f64, args[5].of.f64, args[6].of.f64, args[7].of.f64);

  _current_draw->drawCircle(center, radius, color);

  return nullptr;
}

wasm_trap_t* UiPanel::drawRing(ScriptInstance*, const wasm_val_t args[],
                               wasm_val_t[]) {
  glm::vec2 center = glm::vec2(args[1].of.f64, args[2].of.f64);
  float inner_radius = args[3].of.f64;
  float outer_radius = args[4].of.f64;
  glm::vec4 color =
      glm::vec4(args[5].of.f64, args[6].of.f64, args[7].of.f64, args[8].of.f64);

  _current_draw->drawRing(center, inner_radius, outer_radius, color);

  return nullptr;
}

}  // namespace core
}  // namespace mondradiko
