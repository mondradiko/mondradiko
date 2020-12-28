/**
 * Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;

layout(location = 0) out vec3 fragColor;

void main() {
  gl_Position = camera.projection * camera.view * vec4(vertPosition, 1.0);

  fragColor = vertColor;
}
