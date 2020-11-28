/**
 * Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 1) const int TEXTURE_NUM = 128;

layout(push_constant) uniform push_constants_t {
  uint view_index;
  uint material_index;
} push_constants;

layout(set = 0, binding = 1) uniform sampler2D textures[TEXTURE_NUM];

struct MaterialUniform {
  uint albedo_texture;
};

layout(set = 0, binding = 2) uniform MaterialUniformArray {
  MaterialUniform materials[128];
} materials;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = texture(textures[materials.materials[push_constants.material_index].albedo_texture], fragTexCoord);  // * vec4(fragColor, 1.0);
}
