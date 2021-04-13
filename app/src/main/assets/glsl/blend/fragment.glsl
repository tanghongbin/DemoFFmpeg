#version 300 es
precision mediump float;                            
in vec2 v_texCoord;                                 
layout(location = 0) out vec4 outColor;             
uniform sampler2D s_TextureMap;                     
void main()                                         
{
  vec4 color = texture(s_TextureMap, v_texCoord);
  float avavege = (color.r + color.g + color.b)/3.0;
//  outColor = vec4(avavege,avavege,avavege,1.0);
  outColor = color;
}