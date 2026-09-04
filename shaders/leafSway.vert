#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float u_Time;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    vec3 pos = aPos;

    // wind sway: displacement grows with height (aPos.y), so the base barely
    // moves and the tip swings more. Tweak the constants to taste.
    float sway = sin(u_Time * 2.0 + aPos.y * 3.0) * 0.05 * aPos.y;
    pos.x += sway;
    pos.z += sway * 0.5; // slight figure-eight motion instead of flat side-to-side

    gl_Position = u_Projection * u_View * u_Model * vec4(pos, 1.0f);
    FragPos = vec3(u_Model * vec4(pos, 1.0f));
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    TexCoords = aTexCoords;
}