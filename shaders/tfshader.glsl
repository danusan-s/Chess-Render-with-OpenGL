#version 330 core

out vec4 FragColor;

in vec2 texCord;

uniform sampler2D ourTexture;

void main()
{   
    FragColor = texture(ourTexture,texCord);
}