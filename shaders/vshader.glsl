#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(2.0*aPos.x - 1.0,2.0*aPos.y - 1.0 , 0.0, 1.0);
    ourColor = aColor;
}