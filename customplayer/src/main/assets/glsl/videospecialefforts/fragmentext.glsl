#version 300 es
//OpenGL ES3.0外部纹理扩展
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
layout(location = 0) out vec4 outColor;


in vec2 out_texCoords;
uniform sampler2D s_TextureMap0;
uniform sampler2D s_TextureMap1;
uniform sampler2D s_TextureMap2;
uniform int custom_type;
uniform float s_alpha;

uniform samplerExternalOES yuvTexSampler;

void main()
{
//  outColor = texture(s_TextureMap, out_texCoords);
//  outColor = vec4(0.0,0.0,1.0,1.0);
  outColor = texture(yuvTexSampler, out_texCoords);
}
