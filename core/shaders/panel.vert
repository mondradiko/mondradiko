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

const vec2 vert_positions[] = {
  vec2(-0.5, -0.5),
  vec2( 0.5, -0.5),
  vec2(-0.5,  0.5),
  vec2( 0.5,  0.5)
};

layout(location = 0) out vec4 fragColor;

void main() {
  PanelUniform panel = panels.panels[gl_InstanceIndex];

  vec4 vert_position = vec4(vert_positions[gl_VertexIndex] * panel.size, 0.0, 1.0);
  gl_Position = camera.projection * camera.view * panel.transform * vert_position;
  fragColor = panel.color;
  fragColor.rgb *= fragColor.a;
}
