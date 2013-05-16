#version 130

#ifdef GL_ES
precision mediump float;
#endif

in vec2 frag_texcoord;

uniform sampler2D nightTexture;

uniform float time;

out vec4 fragColor;

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

void main( void ) {
    float color = pow(noise(gl_FragCoord.xy), 10.0);// * 20.0;

    vec2 seed1 = vec2(cos(time * 0.00005), sin(time*.000005));
    vec2 seed2 = vec2(cos(time*.000001), sin(time*.0000001));
    vec2 seed3 = vec2(sin(time*.000001), cos(time*.0000001));

    float r1 = noise(gl_FragCoord.xy*noise(seed1));
    float r2 = noise(gl_FragCoord.xy*noise(seed2));
    float r3 = noise(gl_FragCoord.xy*noise(seed3));

    fragColor = vec4(vec3(color*r1, color*r2, color*r3), 1.0);
    fragColor.a += frag_texcoord.x * 0.00000000000001;
}
