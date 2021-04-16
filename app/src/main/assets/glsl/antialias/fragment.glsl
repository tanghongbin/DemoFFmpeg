#version 300 es                              
precision mediump float;   
in vec3 v_normal;                     
layout(location = 0) out vec4 outColor;
uniform samplerCube s_texture;
void main()                                  
{                                            
   outColor = vec4(vec3(0.0,1.0,0.0),1.0);
}