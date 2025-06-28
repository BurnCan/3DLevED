
#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightDir;    // e.g., vec3(0.5, 1.0, 0.3)
uniform vec3 lightColor;  // e.g., vec3(1.0)
uniform vec3 objectColor; // e.g., vec3(0.8, 0.2, 0.2)

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDirNorm = normalize(lightDir);

    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = diffuse * objectColor;
    FragColor = vec4(result, 1.0);
}