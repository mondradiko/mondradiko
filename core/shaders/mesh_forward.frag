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

struct PointLightUniform {
  vec4 position;
  vec4 intensity;
};

layout(set = 2, binding = 1) buffer readonly LightUniforms {
  PointLightUniform point_lights[];
} lights;

layout(set = 3, binding = 0) uniform sampler2D albedo_texture;
layout(set = 3, binding = 1) uniform sampler2D emissive_texture;
layout(set = 3, binding = 2) uniform sampler2D normal_map_texture;
layout(set = 3, binding = 3) uniform sampler2D metal_roughness_texture;

layout(location = 0) flat in uint fragMesh;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

float D_GGX(float NoH, float a) {
  float a2 = a * a;
  float f = (NoH * a2 - NoH) * NoH + 1.0;
  return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
  return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
  float a2 = a * a;
  float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
  float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
  return 0.5 / (GGXV + GGXL);
}

float Fd_Lambert() {
  return 1.0 / PI;
}

vec3 BRDF(vec3 l,         // Normalized light direction
          vec3 n,         // Normalized surface normal
          vec3 v,         // Normalized view direction
          vec3 albedo,    // Surface albedo
          float metallic, // Surface metallic
          float p_rough   // Perceptual (artist-defined) surface roughness
) {
  // Props to the Google Filament team for sharing their wonderful PBR math!

  vec3 h    = normalize(v + l);

  float NoL = clamp(dot(n, l), 0.0, 1.0);
  float NoV = abs(dot(n, v)) + 1e-5;
  float NoH = clamp(dot(n, h), 0.0, 1.0);
  float LoH = clamp(dot(l, h), 0.0, 1.0);

  // Map perceptual roughness to real roughness
  float roughness = p_rough * p_rough;

  float a = NoH * roughness;
  vec3 f0 = vec3(0.04);   // Default to 4% reflectance at normal incidence

  float D = D_GGX(NoH, a);
  float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
  vec3  F = F_Schlick(LoH, f0);

  // Specular BRDF
  vec3 Fr = (D * V) * F;

  // Diffuse BRDF
  vec3 Fd = albedo * Fd_Lambert();

  return Fr + Fd;
}

vec3 getNormal(in MaterialUniform material) {
  if (material.normal_map_scale > 0.0) {
    vec3 sampled_normal = texture(normal_map_texture, fragTexCoord).rgb * 2 - 1;
    sampled_normal *= vec3(vec2(material.normal_map_scale), 1.0);
    sampled_normal = normalize(sampled_normal);

    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent);
    vec3 B = cross(T, N);

    mat3 tangent_space = mat3(T, B, N);
    return tangent_space * sampled_normal;
  } else {
    return normalize(fragNormal);
  }
}

vec3 getAlbedo(in MaterialUniform material) {
  vec3 surface_albedo = material.albedo_factor.rgb;

  vec4 sampled_albedo = texture(albedo_texture, fragTexCoord);
  surface_albedo *= sampled_albedo.rgb;

  return surface_albedo;
}

void main() {
  MeshUniform mesh = meshes.meshes[fragMesh];
  MaterialUniform material = materials.materials[mesh.material_idx];

  if (material.enable_blend) {
    outColor = vec4(1.0, 0.0, 1.0, 1.0);
    return;
  }

  vec3 surface_albedo = getAlbedo(material);

  if (material.is_unlit) {
    outColor = vec4(surface_albedo, 1.0);
    return;
  }

  float surface_metallic = material.metallic_factor;
  float surface_roughness = material.roughness_factor;

  if (material.has_metal_roughness_texture) {
    vec2 metal_rough = texture(metal_roughness_texture, fragTexCoord).bg;
    surface_metallic *= metal_rough.x;
    surface_roughness *= metal_rough.y;
  }

  vec3 surface_position = fragPosition;
  vec3 N = getNormal(material);
  vec3 V = normalize(camera.position - surface_position);

  vec3 surface_luminance = vec3(0.05) * surface_albedo;

  for (uint i = 0; i < mesh.light_count; i++) {
    vec3 light_position = lights.point_lights[i].position.xyz - surface_position;
    vec3 light_intensity = lights.point_lights[i].intensity.rgb;
    vec3 light_direction = normalize(light_position);
    vec3 radiance = light_intensity / dot(light_position, light_position);

    vec3 reflected = BRDF(light_direction, N, V,
                          surface_albedo, surface_metallic, surface_roughness);

    surface_luminance += radiance * reflected;
  }

  // TODO(marceline-cramer): Fix PBR so that tone mapping doesn't make JPEG bad
  vec3 tone_mapped = surface_luminance / (surface_luminance + vec3(1.0));

  outColor = vec4(tone_mapped, 1.0);
}
