#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4[6] u_lightTransforms;

// World space of vertex
out vec4 FragPos;

void main() {
    // Emit triangle for each perspective of light cube
    for(int face = 0; face < 6; face++) {
        gl_Layer = face;
        for(int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position; /* = u_M * pos */
            gl_Position = u_lightTransforms[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}