#version 300 es
//OpenGL ES3.0外部纹理扩展
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
layout(location = 0) out vec4 outColor;


in vec2 out_texCoords;
uniform samplerExternalOES yuvTexSampler;

void main()
{
  outColor = texture(yuvTexSampler, out_texCoords);
//  outColor = vec4(vec3((color.r+color.g+color.b)/3),1.0);
//  outColor = vec4(0.5,1.0,1.0,1.0);
}
