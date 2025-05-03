#version 410 core

in vec2 iUV;

out vec4 outColor;

uniform sampler2D utex;

void main(){
    vec2 uv = iUV; uv.y = 1. - uv.y;
    outColor = texture(utex, uv);
    if (outColor.a < 1.) discard;
    // outColor = vec4(1.);
}
