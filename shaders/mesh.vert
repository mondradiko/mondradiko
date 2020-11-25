#version 450
#extension GL_ARB_separate_shader_objects : enable

// TODO(marceline-cramer) Overwrite this with more viewports when they are added
layout(constant_id = 0) const int CAMERA_NUM = 2;

layout(push_constant) uniform push_constants_t {
  uint view_index;
  uint material_index;
} push_constants;

layout(set = 0, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 projection;
} cameras[CAMERA_NUM];

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
  uint view_index = push_constants.view_index;
  gl_Position = cameras[view_index].projection * cameras[view_index].view * vec4(vertPosition + vec3(0.0, 2.0, -2.0), 1.0);

  fragColor = vec3(vertTexCoord, 0.0);
  fragTexCoord = vertTexCoord;
}
