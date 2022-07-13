#version 450 core

// Model view projection matrix
layout(std140, binding = 1) uniform Transformations {
    mat4 u_VP;
    mat4 u_M;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_normal;
out vec3 v_pos;
out vec2 v_textureUV;

void main() {
    vec4 worldPos = u_M * vec4(position, 1.0);
    gl_Position = u_VP * worldPos;

    v_pos = vec3(worldPos);

    // Gets rid of translation from u_M
    // And handles non-uniform scaling
    mat3 normalTransform = transpose(inverse(mat3(u_M)));
    v_normal = normalTransform * normal;

    v_textureUV = texCoord;
}