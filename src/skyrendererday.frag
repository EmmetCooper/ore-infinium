#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform float time;
uniform float height;

uniform sampler2D dayTexture;
uniform sampler2D duskTexture;

out vec4 fragColor;

void main() {
    vec3 final = mix(texture2D(dayTexture, frag_texcoord).rgb, (texture2D(duskTexture, frag_texcoord).rgb), time);

    fragColor = frag_color * vec4(final, 1.0);
}
