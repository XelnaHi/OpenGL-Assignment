#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec3 posColor;

void main()
{
//   FragColor = vec4(ourColor, 1.0f);
   FragColor = vec4(posColor, 1.0f);
}