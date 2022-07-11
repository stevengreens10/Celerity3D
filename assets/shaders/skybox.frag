#version 450 core

uniform samplerCube u_cubeTex;
layout(location = 0) out vec4 fragColor;
in vec3 texDirection;

void main() {
    vec3 color = (texDirection * 0.5) + 0.5;
    fragColor = texture(u_cubeTex, texDirection);
//    fragColor = vec4(color, 1);
}