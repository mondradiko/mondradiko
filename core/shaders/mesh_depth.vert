// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
    vec3 position;
} camera;

layout(set = 3, binding = 0) uniform MeshUniform {
  mat4 model;
  uint light_count;
} mesh;

layout(location = 0) in vec3 vertPosition;

void main() {
  gl_Position = camera.projection * camera.view * mesh.model * vec4(vertPosition, 1.0);
}
