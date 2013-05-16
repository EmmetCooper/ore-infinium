#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform float timeOrig;

uniform sampler2D nightTexture;
uniform sampler2D dayTexture;
uniform sampler2D duskTexture;

out vec4 fragColor;

void main()
{
    vec4 dayColor = texture2D(dayTexture, frag_texcoord);
    vec4 duskColor = texture2D(duskTexture, frag_texcoord);
    vec4 nightColor = texture2D(nightTexture, frag_texcoord);

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
    vec4 darkTop = nightColor;//vec4(0.0, 0.0, 0.0, 1.0);
    vec4 darkBtm = nightColor;//vec4(0.0, 0.0, 0.0, 1.0);

    vec4 top = dayTop;
    vec4 btm = dayBtm;

    top = mix(top, duskTop, toDusk);
    top = mix(top, darkTop, toDark);
    top = mix(top, dayTop, toDay);

    btm = mix(btm, duskBtm, toDusk);
    btm = mix(btm, darkBtm, toDark);
    btm = mix(btm, dayBtm, toDay);

    fragColor = mix(top, btm, 1.0 - (frag_texcoord.y / 1.0)) * frag_color + (dayColor * 0.000000000001 * duskColor * nightColor);
}
