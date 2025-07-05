#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;   // Should be -camera.getFront()
uniform vec3 lightColor; // Usually vec3(1.0)
uniform float time;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // Orbit offset for the pupil center
    float orbitRadius = 0.3;
    vec2 orbitOffset = vec2(
        orbitRadius * cos(time * 2.0),
        orbitRadius * sin(time * 3.0)
    );

    // Pupil center moves around lightDir
    vec3 pupilDir = normalize(lightDir + vec3(orbitOffset, 0.0));

    // Dot product tells us how aligned this fragment is with the pupil center
    float pupilAlignment = dot(norm, pupilDir);
    float pupilDistance = 1.0 - pupilAlignment;

    // Thresholds
    float pupilRadius = 0.1;
    float irisRadius  = 0.15;

    vec3 scleraColor = vec3(1.0);               // white
    vec3 pupilColor  = vec3(0.0);               // black
    vec3 irisColor   = vec3(0.2, 0.5, 0.7);     // bluish-green iris ring

    vec3 baseColor;

    if (pupilDistance < pupilRadius)
        baseColor = pupilColor;
    else if (pupilDistance < irisRadius)
        baseColor = irisColor;
    else
        baseColor = scleraColor;

    // Lighting based on camera-facing direction
    float brightness = max(dot(norm, lightDir), 0.2);
    vec3 finalColor = baseColor * brightness * lightColor;

    FragColor = vec4(finalColor, 1.0);
}
