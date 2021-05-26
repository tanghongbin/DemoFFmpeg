#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

void main()
{
  //  outColor = texture(s_TextureMap, v_texCoord);
  outColor = vec4(0.0,0.0,1.0,1.0);
}