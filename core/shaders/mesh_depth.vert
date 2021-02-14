// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
    vec3 position;
} camera;

struct MaterialUniform {
  vec3 emissive_factor;
  vec4 albedo_factor;

  float mask_threshold;
  float metallic_factor;
  float roughness_factor;
  float normal_map_scale;

  bool is_unlit;
  bool enable_blend;
  bool has_emissive_texture;
  bool has_metal_roughness_texture;
};

layout(set = 1, binding = 0) buffer readonly MaterialDescriptor {
  MaterialUniform materials[];
} materials;

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

layout(location = 0) out float fragMask;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragAlphaFactor;

void main() {
  MeshUniform mesh = meshes.meshes[gl_InstanceIndex];

  gl_Position = camera.projection * camera.view * mesh.model * vec4(vertPosition, 1.0);

  MaterialUniform material = materials.materials[mesh.material_idx];
  fragMask = material.mask_threshold;
  fragTexCoord = vertTexCoord;
  fragAlphaFactor = material.albedo_factor.a;
}
