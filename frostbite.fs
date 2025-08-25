// frostbite.fs
#version 330

uniform sampler2D texture0;
uniform float time;

in vec2 fragTexCoord;
out vec4 finalColor;

float noise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 uv = fragTexCoord;

    // Frost spread effect: animated distortion
    float distortion = noise(uv * 10.0 + time * 0.5) * 0.02;
    uv += vec2(distortion, distortion);

    vec4 texColor = texture(texture0, uv);

    // Desaturate
    float gray = dot(texColor.rgb, vec3(0.3, 0.59, 0.11));

    // Frosty tint (light blue)
    vec3 frosty = mix(vec3(gray), vec3(0.7, 0.85, 1.0), 0.6);

    // Animate strength like frost spreading
    float spread = smoothstep(0.0, 1.0, sin(time * 0.5) * 0.5 + 0.5);

    finalColor = mix(texColor, vec4(frosty, texColor.a), spread);
}
