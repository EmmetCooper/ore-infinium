#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform float time;
uniform float height;

uniform sampler2D dayTexture;
uniform sampler2D duskTexture;

out vec4 fragColor;

void main() {
    vec4 final = texture2D(dayTexture, frag_texcoord);

    vec4 day = texture2D(dayTexture, frag_texcoord);
    vec4 dusk = texture2D(duskTexture, frag_texcoord);

    final = mix(day, dusk, time * (frag_texcoord.y / 1.0));

    fragColor = frag_color * (final);
}
