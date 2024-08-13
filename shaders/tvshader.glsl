#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 texCordi;

out vec2 texCord;

void main()
{
    gl_Position = vec4(2.0*aPos.x - 1.0,2.0*aPos.y - 1.0 , 1.0, 1.0);
    texCord = texCordi;
}