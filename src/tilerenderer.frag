#version 130

in vec3 frag_texcoord;
in vec4 frag_color;

uniform sampler2DArray tex;

out vec4 fragColor;

void main() {
    vec4 texel = texture(tex, frag_texcoord);
    fragColor = (frag_color.rgba) * vec4(texel.rgb, texel.a);
}
