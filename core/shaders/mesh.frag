/**
 * Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
    vec3 position;
} camera;

layout(set = 1, binding = 0) uniform MaterialUniform {
  vec4 albedo_factor;
} material;

layout(set = 2, binding = 0) uniform sampler2D albedo_texture;

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

void main() {
  vec4 sampled_albedo = texture(albedo_texture, fragTexCoord);

  vec3 surface_position = fragPosition;
  vec3 surface_normal = normalize(fragNormal);
  vec3 surface_albedo = (sampled_albedo * material.albedo_factor).rgb;
  vec3 view_direction = normalize(camera.position - surface_position);

  vec3 surface_luminance = vec3(0.0); //3) * surface_albedo;

  for (uint i = 0; i < mesh.light_count; i++) {
    vec3 light_position = lights.point_lights[i].position.xyz - surface_position;
    vec3 light_intensity = lights.point_lights[i].intensity.rgb;

    vec3 light_direction = normalize(light_position);
    float light_distance = length(light_position);
    float diffuse = max(dot(light_direction, surface_normal), 0.0);
    vec3 radiance = diffuse * light_intensity / (light_distance * light_distance);

    surface_luminance = surface_luminance + radiance * surface_albedo;
  }

  vec3 tone_mapped = surface_luminance / (surface_luminance + vec3(1.0));
  vec3 gamma_correct = pow(tone_mapped, vec3(1.0/2.2));

  outColor = vec4(gamma_correct, 1.0);
}
