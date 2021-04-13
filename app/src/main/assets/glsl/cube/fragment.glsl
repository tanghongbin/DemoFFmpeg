#version 300 es                              
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec3 TexCoords;
uniform samplerCube cubeMap;

void main()                                  
{
   outColor = texture(cubeMap, TexCoords);
//   outColor = vec4(1.0,1.0,0.0,1.0);
}