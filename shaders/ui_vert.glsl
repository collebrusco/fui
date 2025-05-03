#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 vMinMax;
layout (location = 2) in vec4 uvMinMax;

out vec2 iUV;

void main() {
    iUV = (aPos * (uvMinMax.zw - uvMinMax.xy)) + uvMinMax.xy;
    vec2 pos = (aPos * (vMinMax.zw - vMinMax.xy)) + vMinMax.xy;
    gl_Position = vec4((pos * 2.) - 1., 0.f, 1.0f);
}
