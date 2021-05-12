#version 300 es                              
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec3 Normal;
in vec3 Position;
uniform samplerCube  cubeMap;
uniform vec3 cameraPos;
void main()                                  
{
   float ration = 1.0/1.52;
   vec3 direction = normalize(Position - cameraPos);
   vec3 refelect = refract(direction,normalize(Normal),ration);
//   vec3 refelect = reflect(direction,normalize(Normal));
   outColor = vec4(texture(cubeMap, refelect).rgb,1.0);
}