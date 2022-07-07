#version 460 core

// Model view projection matrix
layout(std140, binding = 1) uniform Transformations {
    mat4 u_VP;
    mat4 u_M;
};
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

void main() {
    gl_Position = u_VP * u_M * position;
}