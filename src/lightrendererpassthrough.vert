#version 130

in vec2 position;
in vec2 texcoord;

uniform mat4 mvp;

in vec4 color;

out vec2 frag_texcoord;
out vec4 frag_color;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    frag_texcoord = texcoord;
    frag_color = color;
}
