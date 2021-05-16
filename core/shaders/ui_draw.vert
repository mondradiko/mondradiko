// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

struct PanelUniform {
  mat4 transform;
  vec4 color;
  vec2 size;
};

layout(set = 1, binding = 0) buffer readonly PanelDescriptor {
  PanelUniform panels[];
} panels;

layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec4 vertColor;

layout(location = 0) out vec4 fragColor;

void main() {
  PanelUniform panel = panels.panels[gl_InstanceIndex];

  vec4 vert_position = vec4(vertPosition, 0.0, 1.0);
  gl_Position = camera.projection * camera.view * panel.transform * vert_position;
  fragColor = vertColor;
  fragColor.rgb *= fragColor.a;
}
