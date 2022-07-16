#version 450 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2

in vec4 FragPos;

flat in vec3 lightPos;
flat in int lightType;
uniform float u_farPlane;

void main() {
    if(lightType == POINT) {
        float lightDistance = length(FragPos.xyz - lightPos);
//         Map to [0, 1] by dividing far plane
        lightDistance /= u_farPlane;

        gl_FragDepth = lightDistance;
    } else if(lightType == DIRECTIONAL) {
        gl_FragDepth = gl_FragCoord.z;
    }
}