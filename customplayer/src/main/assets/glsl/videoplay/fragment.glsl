#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

in vec2 out_texCoords;
uniform sampler2D s_TextureMap0;
uniform sampler2D s_TextureMap1;
uniform sampler2D s_TextureMap2;

void main()
{
//  outColor = texture(s_TextureMap, out_texCoords);
//  outColor = vec4(0.0,0.0,1.0,1.0);
  vec3 yuv;
  yuv.x = texture(s_TextureMap0, out_texCoords).r;
  yuv.y = texture(s_TextureMap1, out_texCoords).r - 0.5;
  yuv.z = texture(s_TextureMap2, out_texCoords).r - 0.5;
  highp vec3 rgb = mat3(1.0,       1.0,     1.0,
  0.0, -0.344, 1.770,
  1.403,  -0.714,     0.0) * yuv;
  outColor = vec4(rgb, 1.0);
}
