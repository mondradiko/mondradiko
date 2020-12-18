/**
 * Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

layout(set = 3, binding = 0) uniform MeshUniform {
  mat4 model;
} mesh;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
  gl_Position = camera.projection * camera.view * mesh.model * vec4(vertPosition, 1.0);

  fragColor = vertColor;
  fragTexCoord = vertTexCoord;
}
