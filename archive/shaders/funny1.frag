#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

void main()
{
    // Normalize coordinates
    vec2 uv = gl_FragCoord.xy / resolution;

    // Wavy color effect
    float wave = sin(uv.x * 10.0 + time * 5.0) + cos(uv.y * 10.0 + time * 3.0);

    // Rainbow cycling colors
    vec3 color = vec3(
        0.5 + 0.5 * sin(time + uv.x * 10.0),
        0.5 + 0.5 * sin(time + uv.y * 10.0 + 2.0),
        0.5 + 0.5 * sin(time + uv.x * 10.0 + 4.0)
    );

    // Mouse sparkle disco zone!
    float d = distance(uv, mouse / resolution);
    float sparkle = 1.0 - smoothstep(0.0, 0.3, d);

    FragColor = vec4(color + sparkle * wave * 0.2, 1.0);
}
