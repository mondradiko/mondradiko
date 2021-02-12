// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
    vec3 position;
} camera;

layout(set = 1, binding = 0) uniform MaterialUniform {
  vec3 emissive_factor;
  bool has_emissive_texture;

  vec4 albedo_factor;

  // Metallic-roughness model
  float metallic_factor;
  float roughness_factor;
  bool has_metal_roughness_texture;
} material;

layout(set = 2, binding = 0) uniform sampler2D albedo_texture;
layout(set = 2, binding = 1) uniform sampler2D emissive_texture;

// Metallic-roughness model
layout(set = 2, binding = 2) uniform sampler2D metal_roughness_texture;

layout(set = 3, binding = 0) uniform MeshUniform {
  mat4 model;
  uint light_count;
} mesh;

struct PointLightUniform {
  vec4 position;
  vec4 intensity;
};

layout(set = 3, binding = 1) buffer readonly LightUniforms {
  PointLightUniform point_lights[];
} lights;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main() {
  vec4 sampled_albedo = texture(albedo_texture, fragTexCoord);
  vec3 surface_albedo = (sampled_albedo * material.albedo_factor).rgb;

  float surface_metallic = material.metallic_factor;
  float surface_roughness = material.roughness_factor;

  if (material.has_metal_roughness_texture) {
    vec2 metal_rough = texture(metal_roughness_texture, fragTexCoord).bg;
    surface_metallic *= metal_rough.x;
    surface_roughness *= metal_rough.y;
  }

  vec3 surface_position = fragPosition;
  vec3 N = normalize(fragNormal);
  vec3 V = normalize(camera.position - surface_position);

  vec3 surface_luminance = vec3(0.01) * surface_albedo;

  vec3 surface_emissive = material.emissive_factor;
  if (material.has_emissive_texture) {
    surface_emissive *= texture(emissive_texture, fragTexCoord).rgb;
  }

  surface_luminance += surface_emissive;

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, surface_albedo, surface_metallic);

  for (uint i = 0; i < mesh.light_count; i++) {
    vec3 light_position = lights.point_lights[i].position.xyz - surface_position;
    vec3 light_intensity = lights.point_lights[i].intensity.rgb;

    vec3 L = normalize(light_position);
    vec3 H = normalize(V + L);
    float light_distance = length(light_position);
    vec3 radiance = light_intensity / (light_distance * light_distance);

    // Cook-Torrance BRDF
    // TODO(marceline-cramer): All of the PBR math here is wrong; please fix
    float NDF = DistributionGGX(N, H, surface_roughness);
    float G = GeometrySmith(N, V, L, surface_roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - surface_metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    float NdotL = max(dot(N, L), 0.0);
    surface_luminance += (kD * surface_albedo / PI + specular) * radiance * NdotL;
  }

  // TODO(marceline-cramer): Fix PBR so that tone mapping doesn't make JPEG bad
  // vec3 tone_mapped = surface_luminance / (surface_luminance + vec3(1.0));
  // vec3 gamma_correct = pow(tone_mapped, vec3(1.0/2.2));

  outColor = vec4(surface_luminance, 1.0);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
