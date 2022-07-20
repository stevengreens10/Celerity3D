#version 450 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2

// Model view projection matrix
layout(std140, binding = 1) uniform Transformations {
    mat4 u_VP;
    mat4 u_M;
};

struct LightSource {
    int type;
    int idx;
    mat4 spaceTransform[6];
    vec3 pos;
    vec3 dir;
    vec3 color;
    vec3 intensities;
    vec3 attenuation;
};

// Scene properties
readonly layout(std430, binding=2) buffer Scene {
    vec3 cameraPos;
    int numLights;
    LightSource lights[];
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;

out vec3 v_normal;
out vec3 v_pos;
out vec2 v_textureUV;

out mat3 TBN;

void main() {
    vec4 worldPos = u_M * vec4(position, 1.0);
    gl_Position = u_VP * worldPos;

    v_pos = vec3(worldPos);

    // Gets rid of translation from u_M
    // And handles non-uniform scaling
    mat3 normalTransform = transpose(inverse(mat3(u_M)));
    v_normal = normalTransform * normal;
    vec3 T = normalTransform * tangent;
    vec3 B = cross(normal, tangent);

    TBN = mat3(T, B, v_normal);

    v_textureUV = texCoord;
}