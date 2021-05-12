#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec2 TexCoords;
uniform sampler2D shadowMap;

void main()                                  
{
   vec3 result = vec3(texture(shadowMap, TexCoords).r);
   outColor = vec4(result,1.0);
}
