// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

layout(location = 0) out vec2 fragTexCoord;

const vec2 vertex_positions[] = {
  vec2(-1.0, -1.0),
  vec2( 1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0,  1.0),
};

void main() {
  gl_Position = camera.projection * camera.view * vec4(vertex_positions[gl_VertexIndex], 0.0, 1.0);
  fragTexCoord = vertex_positions[gl_VertexIndex] * 0.5 + 0.5;
}
