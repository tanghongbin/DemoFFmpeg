#version 300 es
//OpenGL ES3.0外部纹理扩展,使用oes纹理采样器采样
//#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
layout(location = 0) out vec4 outColor;


in vec2 out_texCoords;
uniform sampler2D s_TextureMap;

void main()
{
  vec4 color = texture(s_TextureMap, out_texCoords);
  outColor = color;
//  outColor = vec4(0.5,1.0,1.0,1.0);
}
