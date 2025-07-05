#version 330 core

in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    // Basic diffuse lighting
    float intensity = max(dot(normalize(Normal), normalize(-lightDir)), 0.0);

    // Generate rainbow based on normal direction
    vec3 rainbow = 0.5 + 0.5 * sin(Normal * 10.0 + vec3(0.0, 2.0, 4.0));

    // Mix rainbow color with light
    vec3 color = rainbow * intensity * lightColor;

    FragColor = vec4(color, 1.0);
}
