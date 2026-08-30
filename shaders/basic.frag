#version 330 core
// Fragment shader. What do we mean by fragment? A fragment is a pixel on the screen.
// The fragment shader is responsible for determining the color of each pixel on the screen.
// It takes the output from the vertex shader and uses it to determine the final color of each pixel.

in vec3 vertexColor; // "in" means this variable is coming from the vertex shader

out vec4 FragColor; // "out" means this variable is going to the fragment shader

uniform float u_Pulse;

void main()
{
    vec3 color = vertexColor * u_Pulse;
    FragColor = vec4(color, 1.0f);
    // FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
