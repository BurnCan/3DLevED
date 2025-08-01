
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vertexColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    vertexColor = aPos; // Use position as a substitute color
}
