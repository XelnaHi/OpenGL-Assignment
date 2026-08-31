#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec3 posColor;
out vec2 TexCoord;

uniform float u_HorTrans;
uniform mat4 transform;

void main()
{
    if (transform != mat4(0.0f)){

        gl_Position = transform * vec4(aPos, 1.0);
    } else {
        gl_Position = vec4(aPos, 1.0f);
    }
    posColor = aPos;
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}