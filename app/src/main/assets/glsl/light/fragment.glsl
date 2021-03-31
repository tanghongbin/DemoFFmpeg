#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
void main()
{                                                   
//  outColor = vec4(objectColor * lightColor,1.0);
  outColor = vec4(0.0f,1.0f,0.0f,1.0f);

}