#version 460 core

// Model view projection matrix
uniform mat4 u_VP;
uniform mat4 u_M;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 v_normal;
out vec3 v_pos;

void main() {
    vec4 worldPos = u_M * position;
    gl_Position = u_VP * worldPos;

    v_pos = vec3(worldPos.xyz);

    // Gets rid of translation from u_M
    // And handles non-uniform scaling
    mat3 normalTransform = transpose(inverse(mat3(u_M)));
    v_normal = normalTransform * normal;
}