// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} camera;

struct GlyphData {
  vec2 atlas_coords[4];
  vec2 glyph_coords[4];
};

layout(set = 1, binding = 1) buffer readonly GlyphSet {
  GlyphData data[];
} glyphs;

layout(location = 0) in vec2 glyph_position;
layout(location = 1) in uint glyph_index;

layout(location = 0) out vec2 fragTexCoord;

void main() {
  gl_Position = camera.projection * camera.view * vec4(glyphs.data[glyph_index].glyph_coords[gl_VertexIndex].xy + glyph_position, 0.0, 1.0);
  fragTexCoord = glyphs.data[glyph_index].atlas_coords[gl_VertexIndex].xy;
}
