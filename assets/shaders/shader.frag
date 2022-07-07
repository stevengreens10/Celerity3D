#version 450 core

uniform sampler2D u_Texture;

layout(location = 0) out vec4 color;
in vec2 v_texCoord;

void main() {
	vec4 texColor = texture(u_Texture, v_texCoord);
	color = texColor;
}