#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;       // -camera.getFront()
uniform vec3 lightColor;     // usually vec3(1.0)
uniform float time;          // Pass in seconds (used for wobble)

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // 🌀 Eye follows camera, but pupil wobbles
    float eyeFacing = dot(norm, lightDir);
    float irisDist = 1.0 - eyeFacing;

    // 🎯 Offset center of pupil with wobbly sin() function
    vec2 wobble = vec2(
        0.1 * sin(time * 3.0 + FragPos.x * 10.0),
        0.1 * cos(time * 2.5 + FragPos.y * 12.0)
    );

    // Compute direction from "wobble offset" instead of center
    vec3 wobbleCenter = normalize(lightDir + vec3(wobble, 0.0));
    float wobbleFacing = dot(norm, wobbleCenter);
    float wobbleIrisDist = 1.0 - wobbleFacing;

    // 🎨 Color zones
    vec3 pupilColor = vec3(0.0);             // black
    vec3 irisColor  = vec3(0.2, 0.6, 0.4);    // googly green
    vec3 scleraColor = vec3(1.0);            // white

    vec3 baseColor;
    if (wobbleIrisDist < 0.1)
        baseColor = pupilColor;
    else if (wobbleIrisDist < 0.3)
        baseColor = irisColor;
    else
        baseColor = scleraColor;

    // 🔆 Shading for realism
    float brightness = max(dot(norm, lightDir), 0.1);
    vec3 finalColor = baseColor * brightness * lightColor;

    FragColor = vec4(finalColor, 1.0);
}
