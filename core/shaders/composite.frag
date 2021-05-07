// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputHdr;
layout(input_attachment_index = 0, set = 1, binding = 1) uniform subpassInput inputOverlay;

layout(location = 0) out vec4 outColor;

void main() {
  vec3 luminance = subpassLoad(inputHdr).rgb;
  vec3 tone_mapped = luminance / (luminance + vec3(1.0));

  vec4 overlay = subpassLoad(inputOverlay);

  outColor = vec4(tone_mapped, 1.0);
  outColor *= 1.0 - overlay.a;
  outColor += overlay;
}
