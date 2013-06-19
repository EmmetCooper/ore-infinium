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
   x = x*12 + gl_VertexID*1 + seed*1;
   x = (x >> 13) ^ x;
   return ((x * (x * x * 2 + 19990303) + 1) & 0x7fffffff)/float(0x7fffffff-1);
}

void main() {
   outvelocity = invelocity;
   for(int j = 0;j<1;++j) {
       vec3 diff = inposition-center[j];
       float dist = length(diff);
       float vdot = dot(diff, invelocity);
       if(dist<radius[j] && vdot<0.0)
           outvelocity -= bounce*diff*vdot/(dist*dist);
   }
   outvelocity += dt*g;
   outposition = inposition + dt*outvelocity;
   if(outposition.y < -10.0)
   {
   }
}
