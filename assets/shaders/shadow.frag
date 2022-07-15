#version 450 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2

in vec4 FragPos;

layout(location = 0) vec4 color;

uniform vec3 u_lightPos;
uniform int u_lightType;
uniform float u_farPlane;

void main() {
    if(u_lightType != DIRECTIONAL) {
        float lightDistance = length(FragPos.xyz - u_lightPos);
        // Map to [0, 1] by dividing far plane
        lightDistance /= u_farPlane;

        gl_FragDepth = lightDistance;
    }
}