#version 300 es
precision mediump float;
out vec4 FragColor;
uniform vec3 temparyLightColor;
void main()                                         
{                                                   
//  FragColor = vec4(1.0f,1.0f,1.0f,1.0f);
  FragColor = vec4(temparyLightColor,1.0);
}