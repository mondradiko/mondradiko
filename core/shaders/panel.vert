// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

const vec2 vert_positions[] = {
  vec2(-1.0, -1.0),
  vec2( 1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0,  1.0)
};

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 vert_position = vec4(vert_positions[gl_VertexIndex], 0.0, 1.0);
  gl_Position = camera.projection * camera.view * vert_position;
  fragColor = vec4(0.1, 0.02, 0.02, 0.8);
}
