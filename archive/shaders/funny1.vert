#version 330 core
layout(location = 0) in vec3 aPos;     // Position only
layout(location = 1) in vec3 aNormal;  // Optional, won't be used here

out vec2 TexCoords;

uniform mat4 MVP;

void main()
{
    // Convert 3D position to screen space
    gl_Position = MVP * vec4(aPos, 1.0);

    // Fake UVs using screen-space XY (normalized device coords)
    TexCoords = aPos.xy;
}
