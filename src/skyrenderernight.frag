#version 130

#ifdef GL_ES
precision mediump float;
#endif

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D nightTexture;

out vec4 fragColor;

uniform float time;


#ifdef GL_ES
precision mediump float;
#endif

// Twinkling stars

// Based on work by Trisomie21

//uniform float time;
vec2 resolution = vec2(1,5);


// Tweaked from http://glsl.heroku.com/e#4982.0
float hash( float n ) {
        return fract(sin(n)*43758.5453);
}

float noise( in vec2 x ) {
        vec2 p = floor(x);
        vec2 f = fract(x);
        f = f*f*(3.0-2.0*f);
        float n = p.x + p.y*57.0;
        float res = mix(mix(hash(n+0.0), hash(n+1.0),f.x), mix(hash(n+57.0), hash(n+58.0),f.x),f.y);
        return res;
}

vec3 cloud(vec2 p) {
        float f = 0.0;
        f += 0.50000*noise(p*1.0*20.0);
        f += 0.25000*noise(p*3.0*20.0);
        f += 0.12500*noise(p*4.0*20.0);
        f += 0.06250*noise(p*10.0*12.0);
        f *= f;
        return vec3(f*.65, f*.45, f)*.6;
}

//const float SPEED     = 0.0006;

const float SPEED       = 0.06;
const float BRIGHTNESS  = 10.0;
       vec2 ORIGIN      = resolution.xy*.5;

float rand(vec2 co) {
        return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 437580.5453);
}

void main( void ) {
        vec2   pos = frag_texcoord.xy - ORIGIN;
        float dist = length(pos) / resolution.y;
        vec2 coord = vec2(pow(dist, 0.1), atan(pos.x, pos.y) / (3.1415926*2.0));

        // Nebulous cloud
        vec3 color = vec3(0.0, 0.0, 0.0);//cloud(pos/resolution);

        // Background stars
        float a = pow((1.0-dist),20.0);
        float t = time*.01;
        float r = coord.x - (t*SPEED);
        float c = fract(a+coord.y + 0.0*.543);
        vec2  p = vec2(r, c*.5)*4000.0;
        vec2 uv = fract(p)*10.0-1.0;
        float m = clamp((rand(floor(p))-.9)*BRIGHTNESS, 0.0, 1.0);
        color +=  clamp((1.0-length(uv*2.0))*m*dist, 0.0, 1.0);

        fragColor = vec4(color, 1.0) + (frag_texcoord.x * 0.000000000001 * frag_color.x * time);;;
//    fragColor = vec4(vec3(color*r1, color*r2, color*r3), 1.0) + (frag_texcoord.x * 0.000000000001 * frag_color.x * time);;
}

//    fragColor = vec4(vec3(color*r1, color*r2, color*r3), 1.0) + (frag_texcoord.x * 0.000000000001 * frag_color.x * time);;
