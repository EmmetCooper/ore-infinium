#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform float time;

uniform sampler2D dayTexture;
uniform sampler2D duskTexture;

out vec4 fragColor;

void main() {
vec4 final = mix(texture2D(dayTexture, frag_texcoord), texture2D(duskTexture, frag_texcoord), time);

    fragColor = frag_color * final;
}
