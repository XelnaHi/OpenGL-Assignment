#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec3 posColor;

uniform vec4 u_GreenValue;

void main()
{
   FragColor = vec4(u_GreenValue);
//   FragColor = vec4(posColor, 1.0f);
}