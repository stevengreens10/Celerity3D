#version 460 core

// Model view projection matrix
uniform mat4 u_VP;
uniform mat4 u_M;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

void main() {
    gl_Position = u_VP * u_M * position;
}