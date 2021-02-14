// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
    vec3 position;
} camera;

struct MeshUniform {
  mat4 model;
  uint light_count;
  uint material_idx;
};

layout(set = 2, binding = 0) buffer readonly MeshDescriptor {
  MeshUniform meshes[];
} meshes;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 vertTangent;
layout(location = 3) in vec3 vertColor;
layout(location = 4) in vec2 vertTexCoord;

void main() {
  gl_Position = camera.projection * camera.view * meshes.meshes[gl_InstanceIndex].model * vec4(vertPosition, 1.0);
}
