#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

out vec4 fragColor;
in vec3 Normal;

void main()
{
    outColor = fragColor;
}