/**
 * Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform MaterialUniform {
  uint albedo_texture;
} material;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
  //outColor = texture(textures[material.albedo_texture], fragTexCoord);  // * vec4(fragColor, 1.0);
  outColor = vec4(fragColor, 1.0);
}
