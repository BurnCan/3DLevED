#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

uniform vec3 lightDir;
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    float eyeFacing = dot(norm, lightDir);
    float irisDist = 1.0 - eyeFacing;

    // Calculate distance from center in local XZ plane
    float radial = length(LocalPos.xz);  // or xy if more appropriate
    radial = clamp(radial, 0.0, 1.0);

    // Blue-green variation based on local radial distance
    vec3 dynamicIris = vec3(
        0.05,                            // minimal red
        0.5 + 0.3 * (1.0 - radial),      // more green at center
        0.6 + 0.2 * radial               // more blue at edge
    );

    vec3 pupilColor  = vec3(0.0);
    vec3 irisColor   = dynamicIris;
    vec3 scleraColor = vec3(1.0);

    vec3 baseColor;
    if (irisDist < 0.05)
        baseColor = pupilColor;
    else if (irisDist < 0.15)
        baseColor = irisColor;
    else
        baseColor = scleraColor;

    float brightness = max(dot(norm, lightDir), 0.1);
    vec3 finalColor = baseColor * brightness * lightColor;

    FragColor = vec4(finalColor, 1.0);
}
