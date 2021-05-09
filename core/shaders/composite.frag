// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputHdr;
layout(input_attachment_index = 0, set = 1, binding = 1) uniform subpassInput inputOverlay;

layout(location = 0) out vec3 outColor;

// Converts a color from linear light gamma to sRGB gamma
// From:
// https://gamedev.stackexchange.com/questions/92015/optimized-linear-to-srgb-glsl
// Enhanced for performance
// Removed the alpha calculation
vec3 ConvertLinearToSRgb(vec3 rgb)
{
  bvec3 cutoff = lessThan(rgb, vec3(0.0031308));
  vec3 lower = rgb * vec3(12.92);

  // This branch is OK because lighter fragments will be closer to each other
  // Meaning that most quads/subgroups will take the same branch
  // This is further enhanced by post-processing like FXAA and bloom
  // This has been tested and it shows a definite speed improvement
  if (all(cutoff)) {
    return lower;
  } else {
    vec3 higher = vec3(1.055)*pow(rgb, vec3(1.0/2.4)) - vec3(0.055);
    return mix(higher, lower, cutoff);
  }
}

void main() {
  vec3 luminance = subpassLoad(inputHdr).rgb;
  vec3 tone_mapped = luminance / (luminance + vec3(1.0));

  vec4 overlay = subpassLoad(inputOverlay);

  outColor = ConvertLinearToSRgb(tone_mapped);
  outColor *= 1.0 - overlay.a;
  outColor += overlay.rgb;
}
