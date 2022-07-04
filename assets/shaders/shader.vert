#version 330 core

// Model view projection matrix
uniform mat4 u_MVP;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 v_texCoord; // output a tex coord to the fragment shader

void main() {
    gl_Position = u_MVP * position;
    v_texCoord = texCoord;
}