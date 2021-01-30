// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D glyph_atlas;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
  vec4 sdf = texture(glyph_atlas, fragTexCoord);
  if (median(sdf.r, sdf.g, sdf.b) < 0.5) discard;
  // if (sdf.a < 0.5) discard;

  outColor = vec4(1.0);
}
