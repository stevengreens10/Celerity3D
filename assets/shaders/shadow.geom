#version 450 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4[6] u_lightTransforms;
uniform int u_lightType;
uniform int u_lightIdx;

// World space of vertex
out vec4 FragPos;

void main() {
    int numFaces = 6;
    if (u_lightType == DIRECTIONAL) {
        // Only render with one perspective for directional lights
        numFaces = 1;
    }
    // Emit triangle for each perspective of light cube
    for (int face = 0; face < numFaces; face++) {
        gl_Layer = u_lightIdx*6 + face;
        for (int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position; /* = u_M * pos */
            gl_Position = u_lightTransforms[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}