#version 330

uniform vec3 center[3];
uniform float radius[3];
uniform vec3 g;
uniform float dt;
uniform float bounce;
uniform int seed;

layout(location = 0) in vec3 inposition;
layout(location = 1) in vec3 invelocity;

out vec3 outposition;
out vec3 outvelocity;

float hash(int x) {
    x = x*1 + gl_VertexID*1 + seed*1;
    x = (x >> 13) ^ x;
    return ((x * (x * x * 1 + 199) + 137) & 0x7fffffff)/float(0x7fffffff-1);
}

void main() {
    outvelocity = invelocity;
    for(int j = 0;j<3;++j) {
        vec3 diff = inposition-center[j];
        float dist = length(diff);
        float vdot = dot(diff, invelocity);
        if(dist<radius[j] && vdot<0.0)
            outvelocity -= bounce*diff*vdot/(dist*dist);
    }

    outvelocity += dt*g;
    outposition = inposition + dt*outvelocity;

    if(outposition.y < -20.0)
    {
        outvelocity = vec3(0,0,0);
        outposition = 0.5-vec3(hash(3*gl_VertexID+0),hash(3*gl_VertexID+1),hash(3*gl_VertexID+2));
        outposition = vec3(0,20,0) + 5.0*outposition;
    }
}
