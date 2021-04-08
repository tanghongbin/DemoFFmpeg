#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_TextureMap;

void main()                                         
{
  outColor = vec4(0.04, 0.28, 0.26, 1.0);
}