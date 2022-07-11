#version 450 core
// Model view projection matrix
layout(std140, binding = 1) uniform Transformations {
    mat4 u_VP;
    mat4 u_M;
};

layout(location = 0) in vec3 position;

uniform mat4 lightTransform;

void main() {
    gl_Position = lightTransform * u_M * vec4(position, 1.0f);
}