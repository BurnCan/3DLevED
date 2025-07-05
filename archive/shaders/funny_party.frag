#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;     // Expected to be normalized
uniform vec3 lightColor;   // Usually white
uniform vec3 objectColor;  // Base color

out vec4 FragColor;

void main()
{
    // Dot product gives us brightness based on angle
    float intensity = max(dot(normalize(Normal), normalize(lightDir)), 0.0);

    // � Apply a funny stepped toon shading effect
    float stepped = floor(intensity * 5.0) / 5.0;

    // � Use a rainbow palette based on normal
    vec3 rainbow = vec3(abs(Normal.x), abs(Normal.y), abs(Normal.z));

    // � Final color combines rainbow + stepped toon
    vec3 color = mix(rainbow, objectColor, 0.5) * stepped * lightColor;

    FragColor = vec4(color, 1.0);
}
