#version 450 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2

layout(triangles) in;
layout(triangle_strip, max_vertices=252) out;


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

// World space of vertex
out vec4 FragPos;
out vec3 lightPos;
out int lightType;

void main() {
    for(int l = 0; l < numLights; l++) {
    LightSource light = lights[l];
    int numFaces = 6;
    if (light.type == DIRECTIONAL) {
        // Only render with one perspective for directional lights
        numFaces = 1;
    }
    // Emit triangle for each perspective of light cube
    for (int face = 0; face < numFaces; face++) {
        gl_Layer = light.idx*6 + face;
        for (int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position; /* = u_M * pos */
            lightPos = light.pos;
            lightType = light.type;
            gl_Position = light.spaceTransform[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
    }
}
