#version 450 core

uniform vec3 u_diffuseColor;
layout(location = 0) out vec4 color;

void main() {
	color = vec4(u_diffuseColor, 1.0f);
}