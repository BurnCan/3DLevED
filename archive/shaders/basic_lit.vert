
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 model;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));     // World position
    Normal = mat3(transpose(inverse(model))) * aNormal; // Correctly transformed normal
}
