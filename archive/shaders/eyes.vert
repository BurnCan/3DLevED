#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 MVP;
uniform mat4 model;

out vec3 FragPos;
out vec3 Normal;
out vec3 LocalPos;  // <--- new

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    LocalPos = aPos;  // assuming object is centered and unscaled
    gl_Position = MVP * vec4(aPos, 1.0);
}
