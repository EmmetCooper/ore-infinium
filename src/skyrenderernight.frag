#version 130

#ifdef GL_ES
precision mediump float;
#endif

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D nightTexture;

out vec4 fragColor;

uniform float time;

float random(vec2 ab)
{
        float f = (cos(dot(ab ,vec2(21.9898,78.233))) * 43758.5453);
        return fract(f);
}

float noise(in vec2 xy)
{
        vec2 ij = floor(xy);
        vec2 uv = xy-ij;
        uv = uv*uv*(3.0-2.0*uv);

        float a = random(vec2(ij.x, ij.y ));
        float b = random(vec2(ij.x+1., ij.y));
        float c = random(vec2(ij.x, ij.y+1.));
        float d = random(vec2(ij.x+1., ij.y+1.));
        float k0 = a;
        float k1 = b-a;
        float k2 = c-a;
        float k3 = a-b-c+d;
        return (k0 + k1*uv.x + k2*uv.y + k3*uv.x*uv.y);
}

void main() {
    float color = pow(noise(frag_texcoord.xy), 40.0) * 20.0;

    float r1 = noise(frag_texcoord.xy*noise(vec2(sin(time*0.01))));
    float r2 = noise(frag_texcoord.xy*noise(vec2(cos(time*0.01), sin(time*0.01))));
    float r3 = noise(frag_texcoord.xy*noise(vec2(sin(time*0.05), cos(time*0.05))));

    frag_color = vec4(vec3(color*r1, color*r2, color*r3), 1.0);
}
