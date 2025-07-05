#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;       // -camera.getFront()
uniform vec3 lightColor;     // usually vec3(1.0)
uniform float time;          // pass time in seconds from CPU

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // Distance from the "eye center" aligned to the camera
    float eyeFacing = dot(norm, lightDir);
    float irisDist = 1.0 - eyeFacing;

    // Blue-green iris animation (very little red)
    vec3 dynamicIris = vec3(
        0.1,                                     // minimal red
        0.4 + 0.1 * sin(time * 1.2),             // shifting green
        0.6 + 0.1 * sin(time * 1.5 + 1.0)        // shifting blue
    );

    vec3 pupilColor   = vec3(0.0);   // black center
    vec3 irisColor    = dynamicIris;
    vec3 scleraColor  = vec3(1.0);   // white outer eye

    vec3 baseColor;
    if (irisDist < 0.05)
        baseColor = pupilColor;
    else if (irisDist < 0.15)
        baseColor = irisColor;
    else
        baseColor = scleraColor;

    // Simple lighting
    float brightness = max(dot(norm, lightDir), 0.1);
    vec3 finalColor = baseColor * brightness * lightColor;

    FragColor = vec4(finalColor, 1.0);
}
