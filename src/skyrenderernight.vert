#version 130

uniform mat4 mvp;

in vec2 position;
in vec2 texcoord;

out vec2 frag_texcoord;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    frag_texcoord = texcoord;
}
