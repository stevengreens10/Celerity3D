#version 450 core

layout(std140, binding = 1) uniform Transformations {
    mat4 u_VP;
    mat4 u_M;
};

layout(location=0) in vec3 position;

out vec3 texDirection;

void main() {
    gl_Position = u_VP * u_M * vec4(position, 1.0);
    texDirection = position;
}