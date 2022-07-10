#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    texCoord = uv;
}