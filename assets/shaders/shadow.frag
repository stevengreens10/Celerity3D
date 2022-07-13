#version 450 core

in vec4 FragPos;

uniform vec3 u_lightPos;
uniform float u_farPlane;

void main() {

    float lightDistance = length(FragPos.xyz - u_lightPos);
    // Map to [0, 1] by dividing far plane
    lightDistance /= u_farPlane;

    gl_FragDepth = lightDistance;
}