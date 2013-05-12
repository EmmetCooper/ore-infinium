#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform float timeOrig;
uniform float time;
uniform float height;

uniform sampler2D dayTexture;
uniform sampler2D duskTexture;

out vec4 fragColor;

/*
void main() {

    vec4 dayColor = texture2D(dayTexture, frag_texcoord);
    vec4 duskColor = texture2D(duskTexture, frag_texcoord);

    float maxTime = 24.0;

    float baseLine = mix(0.0, 24.0, timeOrig/maxTime);
    float duskLine = mix(22.0 - 20.0, 24.0 - 20.0, timeOrig/maxTime);

    vec4 final = (dayColor * baseLine) * (duskColor * duskLine);

    final.b += time * 0.00000001;
    fragColor = frag_color * (final);
}
*/

//frag_texcoord.y / 1.0)
//    final.rgb *= frag_texcoord.y * time;

//float val = 0.5 + (timeOrig / 24);
//float val = (time * (frag_texcoord.y / 1.0) * 2.0) + (timeOrig * 0.00000001);
//(timeOrig * (1/25) * 0.5);
void main()
{
    vec4 dayColor = texture2D(dayTexture, frag_texcoord);
    vec4 duskColor = texture2D(duskTexture, frag_texcoord);

    float t = timeOrig;

    float offset = 7.0;
    t = mod( t - offset, 24.0);

    float duskStart = 16.0 - offset;
    float duskEnd   = 19.0 - offset;

    float darkStart = 21.0 - offset;
    float darkEnd   = 24.0 - offset;

    float dayStart  =  4.0 - offset + 24.0;
    float dayEnd    =  7.0 - offset + 24.0;

    float toDusk = (clamp(t, duskStart, duskEnd) - duskStart) / (duskEnd - duskStart);
    float toDark = (clamp(t, darkStart, darkEnd) - darkStart) / (darkEnd - darkStart);
    float toDay  = (clamp(t, dayStart,   dayEnd) -  dayStart) / (dayEnd  -  dayStart);

    vec4 dayTop  = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 dayBtm  = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 duskTop = vec4(0.5, 0.2, 0.0, 1.0);
    vec4 duskBtm = vec4(0.8, 0.8, 0.8, 1.0);
    vec4 darkTop = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 darkBtm = vec4(0.0, 0.0, 0.0, 1.0);

    vec4 top = dayTop;
    vec4 btm = dayBtm;

    top = mix(top, duskTop, toDusk);
    top = mix(top, darkTop, toDark);
    top = mix(top, dayTop, toDay);

    btm = mix(btm, duskBtm, toDusk);
    btm = mix(btm, darkBtm, toDark);
    btm = mix(btm, dayBtm, toDay);

    fragColor = mix(top, btm, 1.0 - (frag_texcoord.y / 1.0));


fragColor.a = fragColor.a + (time * 0.0000000000000001) + (height * 0.0000000000000001) + (frag_color.x * 0.0000000000000001) + (dayColor * duskColor * 0.00000000000000000001) + (timeOrig * 0.0001);
}
