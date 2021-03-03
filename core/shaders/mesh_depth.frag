// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 3, binding = 0) uniform sampler2D albedo_texture;
layout(set = 3, binding = 1) uniform sampler2D emissive_texture;
layout(set = 3, binding = 2) uniform sampler2D normal_map_texture;
layout(set = 3, binding = 3) uniform sampler2D metal_roughness_texture;

layout(location = 0) in float fragMask;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragAlphaFactor;

void main() {
  if (fragMask > 0.0) {
    float sampled_albedo = texture(albedo_texture, fragTexCoord).a;

    if ((sampled_albedo * fragAlphaFactor) <= fragMask) {
      discard;
    }
  }

  // TODO(marceline-cramer) Write velocity buffer
}
