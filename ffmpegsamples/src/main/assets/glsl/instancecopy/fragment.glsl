#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

in vec2 outTexCoords;
uniform sampler2D texture_diffuse1;

void main()
{
    outColor = texture(texture_diffuse1,outTexCoords);
}